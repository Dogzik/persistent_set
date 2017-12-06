#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <cassert>
#include "persistent_set.h"
using namespace std;

template struct persistent_set<string>;

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	int cnt = 0;
	while (cnt < 20) {
		int n = 500 + rand() % 1000;
		int max_n = 10 * n + rand() % (10 * n);
		persistent_set<int, my_linked_ptr> a;
		for (int i = 0; i < n; i++) {
			int b = rand() % max_n;
			a.insert(b);
		}
		cout << "a is initialized after " << static_cast<double>(clock()) / CLOCKS_PER_SEC << " seconds" << endl;
		persistent_set <int, my_linked_ptr> b(a), c;

		std::vector <int> v(max_n);
		for (int i = 0; i < max_n; i++) {
			v[i] = i;
		}
		random_shuffle(v.begin(), v.end());
		for (int i = 0; i < max_n; i++) {
			a.erase(a.find(v[i]));
		}
		cout << "a is clear after "<< static_cast<double>(clock()) / CLOCKS_PER_SEC << " seconds" << endl;
		while (b.begin() != b.end()) {
			b.erase(b.begin());
		}
		cout << "b is clear after " << static_cast<double>(clock()) / CLOCKS_PER_SEC << " seconds" << endl;
		random_shuffle(v.begin(), v.end());
		for (int i = 0; i < max_n; i++) {
			c.erase(c.find(v[i]));
		}
		cout << "c is clear after " << static_cast<double>(clock()) / CLOCKS_PER_SEC << " seconds" << endl;
		assert(a.begin() == a.end());
		assert(b.begin() == b.end());
		assert(c.begin() == c.end());
		cnt++;
		cout << endl;
	}

	int x;
	cin >> x;
	return 0;
}
