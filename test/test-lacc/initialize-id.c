int id(int x) {
	return x;
}

int main(void) {
	int x[] = {id(1), id(0)};
	return x[1];
}
