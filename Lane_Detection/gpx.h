#include "..\tinyxml2\tinyxml2.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <assert.h>

using namespace std;
using namespace tinyxml2;

struct GpxRecord {
	string timeStamp;
	double lon;
	double lat;
	double speed;
	double heading;

	void printOut() {
		cout << setprecision(10) << "lon=" << lon;
		cout << setprecision(10) << " lat=" << lat;
		cout << setprecision(10) << " speed=" << speed;
		cout << setprecision(10) << " heading=" << heading;
		cout << " time=" << timeStamp;
		cout << endl;
	}
};

class GPX {
private:
	XMLDocument xmlDoc;
	XMLNode* gpx;
	XMLElement* trk;
	XMLElement* trkseg;
	XMLElement* trkpt;

public:
	GPX(char* str) {
		XMLError eResult = xmlDoc.LoadFile(str);
		assert(eResult == XML_SUCCESS);
		gpx = xmlDoc.FirstChildElement("gpx");
		assert(gpx != nullptr);
		trk = gpx->FirstChildElement("trk");
		assert(trk != nullptr);
		trkseg = trk->FirstChildElement("trkseg");
		assert(trkseg != nullptr);
		trkpt = trkseg->FirstChildElement("trkpt");
		assert(trkpt != nullptr);
	};

	bool forwardRecordPointer() {
		trkpt = trkpt->NextSiblingElement();
		if (!trkpt)
			return false;
		else
			return true;
	}
	
	double getLon() {
		string text = trkpt->Attribute("lon");
		return atof(text.c_str());
	}

	double getLat() {
		string text = trkpt->Attribute("lat");
		return atof(text.c_str());
	}

	double getSpeed() {
		XMLElement* speed = trkpt->FirstChildElement("speed");
		string text = speed->GetText();
		return atof(text.c_str());
	}

	double getHeading() {
		XMLElement* heading = trkpt->FirstChildElement("heading");
		string text = heading->GetText();
		return atof(text.c_str());
	}

	string getTime() {
		XMLElement* timeStamp = trkpt->FirstChildElement("time");
		string text = timeStamp->GetText();
		//text = text.substr(0, text.size() - 1);
		return text;
	}

	GpxRecord getNextRecord() {
		GpxRecord record;
		if (!forwardRecordPointer()) {
			record.timeStamp = "";
		}
		else
		{
			record.lon = getLon();
			record.lat = getLat();
			record.speed = getSpeed();
			record.heading = getHeading();
			record.timeStamp = getTime();
		}
		return record;
	}

	GpxRecord getRecord() {
		GpxRecord record;
		record.lon = getLon();
		record.lat = getLat();
		record.speed = getSpeed();
		record.heading = getHeading();
		record.timeStamp = getTime();
		return record;
	}
};