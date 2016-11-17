#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <unistd.h>

using namespace std;

class Container {
public:
	size_t K;
	size_t Step;
	size_t Points;
	size_t Experiments;
	vector <size_t> Total;
};

size_t RandSumm(void) {
	size_t a = rand() % 6;
	a++;
	size_t b = rand() % 6;
	b++;
	return a + b;
}

void *ChanseCounter(void *arg) {
	Container *obj = (Container *) arg;

	obj->Total.resize(obj->Experiments);
	for (size_t i = 0; i < obj->Experiments; i++) {
		obj->Total[i] = obj->Points;
		for (size_t j = obj->Step; j < obj->K; j++) {
			obj->Total[i] += RandSumm();
		}
	}
	return (void *) arg;
}

int main() {
	srand(time(NULL));
	Container player1, player2;
	pthread_t thread1, thread2;

	int thread_result[2];
	cout << "Input basic data" << endl;

	size_t k;
	cout << "Summary steps: ";
	cin >> k;
	player1.K = k;
	player2.K = k;

	size_t experiments;
	cout << "Experiments: ";
	cin >> experiments;
	player1.Experiments = experiments;
	player2.Experiments = experiments;

	size_t step;
	cout << "Current step: ";
	cin >> step;
	player1.Step = step;
	player2.Step = step;

	cout << endl << "Input Player1 data" << endl;
	cout << "Points: ";
	cin >> player1.Points;

	cout << endl << "Input Player2 data" << endl;
	cout << "Points: ";
	cin >> player2.Points;

	cout << endl << "Modeling games..." << endl;
	thread_result[0] = pthread_create(&thread1, NULL, ChanseCounter, &player1);
	thread_result[1] = pthread_create(&thread2, NULL, ChanseCounter, &player2);
	sleep(1);

	if (thread_result[1] != 0 || thread_result[1] != 0) {
		cout << "ERROR: Modeling games. Aborting..." << endl;
		return 0;
	}
	cout << "OK" << endl;

	cout << endl << "Counting chanses..." << endl;
	size_t point_summs[] = {0, 0};
	for (size_t i = 0; i < experiments; i++) {
		if (player1.Total[i] > player2.Total[i]) {
			point_summs[0]++;
		} else if (player1.Total[i] < player2.Total[i]) {
			point_summs[1]++;
		}
	}
	double chanses[2];
	for (size_t i = 0; i < 2; i++) {
		chanses[i] = (double) point_summs[i] / experiments * 100.0;
	}
	cout << "OK" << endl << endl;

	cout << "=============================" << endl;
	cout << "RESULTS" << endl;
	cout << "Player1 chanse: " << chanses[0] << "%" << endl;
	cout << "Player2 chanse: " << chanses[1] << "%" << endl;
	cout << endl;

	return 0;
}
