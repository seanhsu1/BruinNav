#include "provided.h"
#include "support.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <algorithm>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
    
private:
    struct Node
    {
        Node(GeoSegment segment, string street, double f, double g, double h, Node* parent)
        {
            this->segment=GeoSegment(segment);
            this->street = street;
            this->f=f;
            this->g=g;
            this->h=h;
            this->parent=parent;
            this->dummy = false;
        }
        GeoSegment segment;
        string street;
        double f;
        double g;
        double h;
        bool dummy;
        Node* parent;
    
    };
    
    //used in priority queue in navigate to order by f values (A*)
    struct CompareNodes {
        bool operator() (Node* arg1, Node* arg2) {
            return arg1->f>arg2->f;
        }
    };
    
    //deallocate all nodes in open and closed lists
    void cleanupNodes(priority_queue<Node*, vector<Node*>, CompareNodes> open,
                      list<Node*> closed) const
    {
        while(!open.empty())
        {
            Node* c = open.top();
            open.pop();
            delete c;
        }
        for (list<Node*>::iterator it=closed.begin(); it != closed.end(); ++it)
        {
            Node* c = *it;
            delete c;
        }
    }
    
    AttractionMapper m_am;
    SegmentMapper m_sm;
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

//initialize attraction mapper and segment mapper
bool NavigatorImpl::loadMapData(string mapFile)
{
    MapLoader m;
    if(!m.load(mapFile))
        return false;
    m_am.init(m);
    m_sm.init(m);
    
	return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{

    //get coordinates of source
    GeoCoord startCoord;
   if(!m_am.getGeoCoord(start, startCoord))
       return NAV_BAD_SOURCE;
    
    //get coordinates of destination
    GeoCoord endCoord;
    if(!m_am.getGeoCoord(end, endCoord))
        return NAV_BAD_DESTINATION;
    

    if(!directions.empty())
        directions.clear();
    
    //used to find an existing Node at given coordinates
    MyMap<GeoCoord,Node*> nodeAt;
    
    //open and closed lists (of A* algorithm)
    priority_queue<Node*, vector<Node*>, CompareNodes> open;
    list<Node*> closed;
    
    bool reachedEnd = false;
   
    //initialize first node at starting point
    GeoSegment gs = GeoSegment(startCoord,startCoord);
    Node* n = new Node(gs,"",0,0,0,nullptr);
    open.push(n);
    nodeAt.associate(n->segment.end, n);


    Node* currentNode;
    //repeat until end point reached, or open list is empty (meaning no path found)
    while(!reachedEnd && !open.empty())
    {
        //get the next node from queue, excluding dummy nodes (where whichlist equals -1)
        do{
            currentNode=open.top();
            open.pop();
            closed.push_back(currentNode);
        } while(currentNode->dummy && !open.empty());
   
        //no more valid nodes from open list
        if(open.empty() && currentNode->dummy)
            return NAV_NO_ROUTE;
        
        currentNode->dummy=true;
        
        
        vector<StreetSegment> s = m_sm.getSegments(currentNode->segment.end);
        //for every segment associated with the currentNode
        for(int i = 0; i<s.size();i++)
        {
            StreetSegment currentStreetSegment = s[i];
            
            //if segment was just traversed
            if(((currentNode->segment.start == currentStreetSegment.segment.start) &&
               (currentNode->segment.end == currentStreetSegment.segment.end)) ||
               ((currentNode->segment.end == currentStreetSegment.segment.start) &&
                (currentNode->segment.start == currentStreetSegment.segment.end)))
               continue;
            
            //check if any attractions on street segment are the end point
            for(int y = 0; y < currentStreetSegment.attractions.size(); y++)
            {
               if(currentStreetSegment.attractions[y].geocoordinates == endCoord) //attraction found
                {
                    reachedEnd = true;
                    double g = distanceEarthMiles(currentNode->segment.end, endCoord) + currentNode->g;
                    Node* n = new Node(GeoSegment(currentNode->segment.end, endCoord),currentStreetSegment.streetName,g,g,0,currentNode);
                    open.push(n);
                    nodeAt.associate(n->segment.end, n);
                    currentNode = n;
                    break;
                }
            }
            
            if(reachedEnd)
                break;
        
            
            GeoCoord points[] = {currentStreetSegment.segment.start, currentStreetSegment.segment.end};
            //for start and endpoints of current segment
            for(int i = 0; i < 2; i++)
            {
                //if coordinate points to the geocoord we just came from
                if (points[i]==currentNode->segment.end)
                    continue;
                
                double g = distanceEarthMiles(currentNode->segment.end, points[i])+currentNode->g;
                double h = distanceEarthMiles(points[i],endCoord);
                double f = g+h;
                
                Node** temp = nodeAt.find(points[i]);
                //if node at point already exists in open or closed list
                if(temp && *temp)
                {
                    //if on open list, but current path to this point is better
                    if(!(*temp)->dummy && f<(*temp)->f)
                    {
                        //replace node with new one
                        (*temp)->dummy=true; //old node becomes dummy node
                        Node* n = new Node(GeoSegment(currentNode->segment.end,points[i]),currentStreetSegment.streetName,f,g,h,currentNode);
                        open.push(n);
                        nodeAt.associate(points[i], n);
                    }
                }
                else
                {
                    //create new node
                    Node* n = new Node(GeoSegment(currentNode->segment.end,points[i]),currentStreetSegment.streetName,f,g,h,currentNode);
                    open.push(n);
                    nodeAt.associate(points[i], n);
                }
            }
        }
    }
    
    if(!reachedEnd)
    {
        //deallocate nodes
        cleanupNodes(open,closed);
        return NAV_NO_ROUTE;
    }
    
    
    //work backwards from endpoint to get directions
    currentNode = *nodeAt.find(endCoord);
    if(!currentNode)
        return NAV_NO_ROUTE; //something went wrong
    
    //while currentNode doesn't point to start node
    while(currentNode->parent)
    {
        //create proceed navsegment
        Node* parent = currentNode->parent;
        string street = currentNode->street;
        GeoSegment geoseg = currentNode->segment;
        double distance = currentNode->g - parent->g;
        string direction  = directionOfLine(geoseg);
    
        NavSegment proceed = NavSegment(direction, street, distance, geoseg);
        directions.push_back(proceed);
        
        //create turn navsegment if appropriate
        if(street!=parent->street && parent->street != "")
        {
            if(angleBetween2Lines(parent->segment, geoseg)<180)
                direction = "left";
            else
                direction = "right";
            NavSegment turn = NavSegment(direction, street);
            directions.push_back(turn);
        }

        currentNode=currentNode->parent;
    }
    reverse(directions.begin(),directions.end());
    
    //deallocate nodes
    cleanupNodes(open,closed);
    
    return NAV_SUCCESS;
}


//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
