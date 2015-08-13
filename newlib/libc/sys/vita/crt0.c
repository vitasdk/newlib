int main(int argc, const char* argv[]);

void _start() {
	exit(main(1, (const char*[]){"", 0}));
}