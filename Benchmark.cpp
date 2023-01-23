#include"ConcurrentAlloc.h"

// ntimes 一轮申请和释放内存的次数
// rounds 轮次
void BenchmarkMalloc(size_t ntimes, size_t nworks, size_t rounds)
{
	std::vector<std::thread> vthread(nworks);
	std::atomic<size_t> malloc_costtime = 0;
	std::atomic<size_t> free_costtime = 0;

	for (size_t k = 0; k < nworks; ++k)
	{
		vthread[k] = std::thread([&, k]() {
			std::vector<void*> v;
			v.reserve(ntimes);

			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					//v.push_back(malloc(16));
					v.push_back(malloc((16 + i) % 8192 + 1));
				}
				size_t end1 = clock();

				size_t begin2 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					free(v[i]);
				}
				size_t end2 = clock();
				v.clear();

				malloc_costtime += (end1 - begin1);
				free_costtime += (end2 - begin2);
			}
			});
	}

	for (auto& t : vthread)
	{
		t.join();
	}
	std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次malloc " << ntimes << ":花费：" << malloc_costtime << std::endl;
	std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次free " << ntimes << ":花费：" << free_costtime << std::endl;
	std::cout<<nworks<<"个线程并发执行"<<rounds<<"轮次，每轮次共 "<<ntimes<<":花费：" << free_costtime+malloc_costtime << std::endl;
/*


	printf("%u个线程并发执行%u轮次，每轮次malloc %u次: 花费：%u ms\n",
		nwork000s, rounds, ntimes, malloc_costtime);
		0
	printf("%u个线程并发执行%u轮次，每轮次free %u次: 花费：%u ms\n",
		nworks, round0s, ntimes, free_costtime);

	printf("%u个线程并发malloc&free %u次，总计花费：%u ms\n",
		nworks, nworks * rounds * ntimes, malloc_costtime + free_costtime);
		*/
}


// 单轮次申请释放次数 线程数 轮次
void BenchmarkConcurrentMalloc(size_t ntimes, size_t nworks, size_t rounds)
{
	std::vector<std::thread> vthread(nworks);
	std::atomic<size_t> malloc_costtime = 0;
	std::atomic<size_t> free_costtime = 0;

	for (size_t k = 0; k < nworks; ++k)
	{
		vthread[k] = std::thread([&]() {
			std::vector<void*> v;
			v.reserve(ntimes);

			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					//v.push_back(ConcurrentAlloc(16));
					v.push_back(ConcurrentAlloc((16 + i) % 8192 + 1));
				}
				size_t end1 = clock();

				size_t begin2 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					ConcurrentFree(v[i]);
				}
				size_t end2 = clock();
				v.clear();

				malloc_costtime += (end1 - begin1);
				free_costtime += (end2 - begin2);
			}
			});
	}

	for (auto& t : vthread)
	{
		//t.detach();
		t.join();
	}
	std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次concurrent alloc " << ntimes << ":花费：" << malloc_costtime << std::endl;
	std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次concurrent dealloc " << ntimes << ":花费：" << free_costtime << std::endl;
	std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次共 " << ntimes << ":花费：" << free_costtime + malloc_costtime << std::endl;
/*
	printf("%u个线程并发执行%u轮次，每轮次concurrent alloc %u次: 花费：%u ms\n",
		nworks, rounds, ntimes, malloc_costtime);

	printf("%u个线程并发执行%u轮次，每轮次concurrent dealloc %u次: 花费：%u ms\n",
		nworks, rounds, ntimes, free_costtime);

	printf("%u个线程并发concurrent alloc&dealloc %u次，总计花费：%u ms\n",
		nworks, nworks * rounds * ntimes, malloc_costtime + free_costtime);
*/
}

int main()
{
	using namespace std;
	int* a = (int*)ConcurrentAlloc(sizeof(int) * 4);
	a[0] = 1;
	a[1] = 2;
	a[2] = 3;
	for (int i = 0; i < 3; i++)
		std::cout << a[i] << std::endl;
	std::cout << std::endl;
	size_t n = 10000;
	std::cout << "==========================================================" << endl;
	BenchmarkConcurrentMalloc(n, 20, 100);
	std::cout << std::endl << endl;

	BenchmarkMalloc(n, 20, 100);
	cout << "==========================================================" << endl;

	return 0;
}