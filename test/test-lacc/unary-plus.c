int printf(const char *, ...);

float g;
long long h = 18446744073709551615ULL;
char c = 'H';

int main(void) {
	g = +(0 * (float) h);
	return printf("%f, %lu\n", g, sizeof(+c));
}
