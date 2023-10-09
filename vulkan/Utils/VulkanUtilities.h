#pragma once

struct QueueFamilyIndices
{
	int graphicsFamily = -1; // Location of Graphics Queue Family
		
	bool isValid()
	{
		return graphicsFamily >= 0;
	}
};