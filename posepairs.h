#ifndef POSEPAIRS_H
#define POSEPAIRS_H

#include <vector>
#include <string>
#include <utility>

const std::vector<std::pair<std::pair<std::string, std::string>, std::string>> posePairs = {
	{{"Neck", "RShoulder"}, "Red"},
	{{"Neck", "LShoulder"}, "Orange Red"},
	{{"RShoulder", "RElbow"}, "Orange"},
	{{"RElbow", "RWrist"}, "Yellow"},
	{{"LShoulder", "LElbow"}, "Orange"},
	{{"LElbow", "LWrist"}, "Yellow"},
	{{"Neck", "RHip"}, "Green Yellow"},
	{{"RHip", "RKnee"}, "Lawn Green"},
	{{"RKnee", "RAnkle"}, "Lime"},
	{{"Neck", "LHip"}, "Aqua Marine"},
	{{"LHip", "LKnee"}, "Turquoise"},
	{{"LKnee", "LAnkle"}, "Cyan"},
	{{"Neck", "Nose"}, "Blue"},
	{{"Nose", "REye"}, "Purple"},
	{{"REye", "REar"}, "Blue Violet"},
	{{"Nose", "LEye"}, "Purple"},
	{{"LEye", "LEar"}, "Blue Violet"}
};

#endif
