#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class MapLoaderImpl
{
public:
    MapLoaderImpl();
    ~MapLoaderImpl();
    bool load(string mapFile);
    size_t getNumSegments() const;
    bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    vector<StreetSegment> m_segments;
};

MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
}

//construct StreetSegments given mapFile
bool MapLoaderImpl::load(string mapFile)
{
    ifstream f;
    
    f.open(mapFile,ifstream::in);
    if(f.fail())
        return false;
    
    for(string line;getline(f,line);)
    {
        //get street name
        string streetname=line;
        getline(f,line);
        
        //get start coordinates
        int pos = line.find(',');
        string la1 = line.substr(0,pos);
        if(line[pos+1]==' ')
            pos++;
        int pos2 = line.find(' ', pos+1);
        string lo1 = line.substr(pos+1, pos2-pos-1);
        
        //get end coordinates
        int pos3 = line.find(',',pos2+1);
        string la2 = line.substr(pos2+1,pos3-pos2-1);
        if(line[pos3+1]==' ')
            pos3++;
        string lo2 = line.substr(pos3+1);
        
        //construct geosegment given start and end coordinates
        GeoSegment segment(GeoCoord(la1,lo1),GeoCoord(la2,lo2));
        
        getline(f,line);
        int na = stoi(line); //number of attractions
        vector<Attraction> attractions;
        for(int i = 0; i < na; i++)
        {
            getline(f,line);
            //get attraction name
            int pos4 = line.find('|');
            string name = line.substr(0,pos4);
            
            //get coordinates
            int pos5= line.find(',',pos4+1);
            string la = line.substr(pos4+1,pos5-pos4-1);
            if(line[pos5+1]==' ')
                pos5++;
            string lo = line.substr(pos5+1);
            
            //construct attraction with name, coordinates, add to vector of attractions
            Attraction at;
            at.name=name;
            at.geocoordinates=GeoCoord(la,lo);
            attractions.push_back(at);
        }
        
        //construct streetsegment with name, attractions, geosegment
        StreetSegment s;
        s.streetName=streetname;
        s.segment=segment;
        s.attractions=attractions;
        m_segments.push_back(s);
    }
    
    f.close();
    return true;
}

//return number of segments
size_t MapLoaderImpl::getNumSegments() const
{
    return m_segments.size();
}

//return true if segNum within bounds
//set StreetSegment equal to segment at segNum
bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if(segNum>=m_segments.size())
        return false;
    seg=m_segments[segNum];
       
    return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
    m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
    delete m_impl;
}

bool MapLoader::load(string mapFile)
{
    return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
    return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
    return m_impl->getSegment(segNum, seg);
}
