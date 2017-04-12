#include "provided.h"
#include "MyMap.h"
#include <string>
#include <cctype>
#include <iostream>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
    void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    MyMap<string,GeoCoord> m_map;
};

AttractionMapperImpl::AttractionMapperImpl()
{
    
}

AttractionMapperImpl::~AttractionMapperImpl()
{

}

//for duplicate attractions, only the last duplicate is kept
void AttractionMapperImpl::init(const MapLoader& ml)
{
    StreetSegment s;
    for(int i = 0; i<ml.getNumSegments(); i++)
    {
        ml.getSegment(i, s);
        for(int y =0; y<s.attractions.size();y++)
        {
            string name;
            //attraction name stored as lowercase
            for(int z = 0; z<s.attractions[y].name.size();z++)
            {
                name+= tolower(s.attractions[y].name[z]);
            }
            m_map.associate(name, s.attractions[y].geocoordinates);
     
        }
    }
    
}

//not case sensitive
//return true if attraction is found
//if found, sets GeoCoord gc to the coordinates of given attraction
bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    string name;
    for(int z = 0; z<attraction.size();z++)
    {
        name+= tolower(attraction[z]);
    }
    const GeoCoord* g = m_map.find(name);
    if(!g)
        return false;
    gc=*g;
    return true;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
