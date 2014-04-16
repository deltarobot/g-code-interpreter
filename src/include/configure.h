#define convertToMm(x) x * 25.4
#define sign(x) ( x >= 0 ? 1 : -1 )

extern double stepRatio;
extern double accelerationMax;
extern double speedMax;
extern int inchMeasurements;
extern int homeDirections[];

extern int configure( char *filename );

