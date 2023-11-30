#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 24, FColor::Red, true);
#define DRAW_SPHERE_COLOR(Location, Color) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 24, Color, true);
#define DRAW_SPHERE_SINGLE_FRAME(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 24, FColor::Red, false, -1.f);
#define DRAW_LINE(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(),StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1);
#define DRAW_LINE_SINGLE_FRAME(StartLocation, EndLocation) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 24, FColor::Red, false, -1.f);
#define DRAW_POINT(Location) if GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, true);
#define DRAW_POINT_SINGLE_FRAME(Location) if GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, false, -1.f);
#define DRAW_CYLINDER(Location) if (GetWorld()) DrawDebugCylinder(GetWorld(), Location, Location + FVector(0.f, 0.f, 50.f), 24, 24, FColor::Blue, true);
#define DRAW_VECTOR(StartLocation, EndLocation) if (GetWorld()) \
	{ \
		DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Red, true); \
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1); \
	}
#define DRAW_VECTOR_SINGLE_FRAME(StartLocation, EndLocation) if (GetWorld()) \
	{\
		DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Red, false, -1.f); \
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.f); \
	}
