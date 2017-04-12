#include "provided.h"
#include "MyMap.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
   MyMap<GeoCoord,vector<StreetSegment>> m_map;
};




SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

//construct associations between geocoordinates and vector of street segments given ml
void SegmentMapperImpl::init(const MapLoader& ml)
{

    StreetSegment s;
    //for every streetsegment from ml
    for(int i = 0; i<ml.getNumSegments(); i++)
    {
        ml.getSegment(i, s);
        
        //associate start coord of segment with segment
        vector<StreetSegment>* v = m_map.find(s.segment.start);
        
        //if vector is already created
        if(v)
            v->push_back(s);
        else
        {
            //otherwise, create a new one
            vector<StreetSegment> v;
            v.push_back(s);
            m_map.associate(s.segment.start, v);
            
        }
        
        //associate end coord of segment with segment
        v = m_map.find(s.segment.end);
        
        //if vector is already created
        if(v)
            
            v->push_back(s);
        else
        {
            //otherwise, create a new one
            vector<StreetSegment> v;
            v.push_back(s);
            m_map.associate(s.segment.end, v);
            
        }
        
        //associate attractions on a current segment with their geocoordinates
        for(int y =0; y<s.attractions.size();y++)
        {
            v = m_map.find(s.attractions[y].geocoordinates);
            if(v)
                v->push_back(s);
            else
            {
                vector<StreetSegment> v;
                v.push_back(s);
                m_map.associate(s.attractions[y].geocoordinates, v);
            
            }
        }
    }
 
}

//returns vector of Streetsegments associated with geocoordinate
//if none, return empty
vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    const vector<StreetSegment>* v = m_map.find(gc);
    if(v)
        return *v;
    else
    {
        return vector<StreetSegment>();
    }
}


//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
