//   Program Taylor0.c
// Compute sine() using the Taylor series with one to five terms.
//    Programmed by Byung Kook Kim

#include <stdio.h>
#include <math.h>

#define TERMS	10

int factorial(int m)
{
	int facto = 1;
	for (int i=2; i<=m; ++i)
		facto = facto*i;

	return facto;
}

int main()
{
  float indeg;
	double inrad, angpwr;
  double s[TERMS], e[TERMS], s0;
	int pwr, facto;
  double pi = atan(1.0) * 4.0;

	// 1. Get input angle
	printf("Enter angle in degrees: ");
	scanf("%g", &indeg);

  inrad = indeg * pi / 180;

	// 2. Compute sine with Math library 
	s0 = sin(inrad);
	printf("sin(%g)= %g using Math library.\n", indeg, s0);

	// 3. Compute sine using the Taylor series up to n terms
	for (int n=1; n<=TERMS; ++n) {
    s[n-1] = 0;
		// Loop to sum term i
		for (int i=1; i<=n; ++i) {
			// Compute pwr
			pwr = 2*i - 1;
			// Compute angle^pwr
			angpwr = inrad;
			for (int j=2; j<=i; ++j) angpwr *= -1*inrad*inrad;
			// Compute factorial pwr!
			facto = factorial(pwr);
			// Compute sine by Taylor series 
			s[n-1] += angpwr/facto;
		}
	}

	// 4. Compute error
	for (int n=1; n<=TERMS; ++n)
		e[n-1] = s[n-1] - s0;

	// 5. Print result
	printf("sin(%f) using Taylor series:\n", indeg);
	for (int i=1; i<=TERMS; ++i) {
		printf("%d terms: sin(%f)= %g, error= %g\n", i, indeg, s[i-1], e[i-1]);
	}

	return 0;
}


