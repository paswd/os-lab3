#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <unistd.h>

using namespace std;

int CharToInt(char sym) {
	int res = 0;
	if (sym >= '0' && sym <= '9') {
		res = sym - '0';
	}
	return res;
}

int StrToInt(char *str) {
	int res = 0;
	for (size_t i = 0; str[i] != '\0'; i++) {
		res *= 10;
		res += CharToInt(str[i]);
	}
	return res;
}

class Container {
public:
	size_t K;
	size_t Step;
	size_t Points;
	size_t Experiments;
	size_t ExperimentNum;
	vector <size_t> Total;
	int MaxHhreads;
};

size_t RandSumm(void) {
	size_t a = rand() % 6;
	a++;
	size_t b = rand() % 6;
	b++;
	return a + b;
}

void *ExperimentModeller(void *arg) {
	Container *obj = (Container *) arg;

	size_t i = obj->ExperimentNum;
	//cout << "ExN: " << i << endl;

	obj->Total[i] = obj->Points;
	for (size_t j = obj->Step; j < obj->K; j++) {
		obj->Total[i] += RandSumm();
	}	

	return arg;
}

void *ChanseCounter(void *arg) {
	const size_t MSEC_CONST = 1000;
	Container *obj = (Container *) arg;

	obj->Total.resize(obj->Experiments);
	vector <pthread_t> threads_tmp;
	threads_tmp.resize(obj->Experiments);

	size_t oldest_thread = 0;
	size_t threads_started = 0;
	size_t max_hreads = obj->MaxHhreads;
	if (max_hreads < 1) {
		max_hreads = 1;
	}
	for (size_t i = 0; i < obj->Experiments; i++) {
		if (threads_started >= obj->MaxHhreads) {
			pthread_join(threads_tmp[oldest_thread], NULL);
			oldest_thread++;
			if (oldest_thread >= obj->Experiments) {
				oldest_thread = obj->Experiments - 1;
			}
			threads_started--;
		}
		obj->ExperimentNum = i;
		pthread_create(&(threads_tmp[i]), NULL, ExperimentModeller, arg);
		threads_started++;
		usleep(MSEC_CONST);
		//sleep(1);
	}

	return arg;
}


int main(int argc, char* argv[]) {
	int threads_cnt = 1;
	if (argc > 1) {
		threads_cnt = StrToInt(argv[1]);
	}
	srand(time(NULL));
	Container player[2];
	pthread_t thread[2];

	int thread_result[2];
	cout << "Input basic data" << endl;
	player[0].MaxHhreads = threads_cnt;
	player[1].MaxHhreads = threads_cnt;

	size_t k;
	cout << "Summary steps: ";
	cin >> k;
	player[0].K = k;
	player[1].K = k;

	size_t experiments;
	cout << "Experiments: ";
	cin >> experiments;
	player[0].Experiments = experiments;
	player[1].Experiments = experiments;

	size_t step;
	cout << "Current step: ";
	cin >> step;
	player[0].Step = step;
	player[1].Step = step;

	cout << endl << "Input Player1 data" << endl;
	cout << "Points: ";
	cin >> player[0].Points;

	cout << endl << "Input Player2 data" << endl;
	cout << "Points: ";
	cin >> player[1].Points;

	cout << endl << "Modeling games..." << endl;
	thread_result[0] = pthread_create(&(thread[0]), NULL, ChanseCounter, &player[0]);
	thread_result[1] = pthread_create(&(thread[1]), NULL, ChanseCounter, &player[1]);
	pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);

	if (thread_result[1] != 0 || thread_result[1] != 0) {
		cout << "ERROR: Modeling games. Aborting..." << endl;
		return 0;
	}
	cout << "OK" << endl;

	cout << endl << "Counting chanses..." << endl;
	size_t point_summs[] = {0, 0};
	for (size_t i = 0; i < experiments; i++) {
		if (player[0].Total[i] > player[1].Total[i]) {
			point_summs[0]++;
		} else if (player[0].Total[i] < player[1].Total[i]) {
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
	for (size_t i = 0; i < experiments; i++) {
		cout << player[0].Total[i] << '\t' << player[1].Total[i] << endl;
	}
	cout << endl;
	cout << "Player1 chanse: " << chanses[0] << "%" << endl;
	cout << "Player2 chanse: " << chanses[1] << "%" << endl;
	cout << endl;

	return 0;
}
