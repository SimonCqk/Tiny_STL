#pragma once
#ifndef TINYSTL_ALLOC_H
#define TINYSTL_ALLOC_H

#include<cstdlib>
#include<iostream>
namespace Tiny_STL {

	 // 空间内存分配器，以字节数为单位

	class alloc {
	private:
		//不同内存块的上调大小
		enum EAligns
		{
			EAlign128 = 8, EAlign256 = 16, EAlign512 = 32,
			EAlign1K = 64, EAlign2K = 128
		};
		enum ENFreeLists { Free_list_num = EAligns::EAlign2K / EAligns::EAlign128 };//free-lists的个数
		enum ENObjs { NOBJS = 20 };//每次增加的节点数
	private:
		//free-lists的节点构造，分配回收小内存区块
		union list_node {
			char client[1];
			union list_node *next;
		};
		static list_node *free_list[ENFreeLists::Free_list_num];	//自由链表
	private:
		static char *start_free;//内存池起始位置
		static char *end_free;//内存池结束位置
		static size_t heap_size;//申请 heap 空间附加值大小
	private:
		//将bytes上调至8的倍数
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes + EAligns::EAlign128 - 1) & ~(EAligns::EAlign128 - 1));
		}
		//根据区块大小，决定使用第n号free-list，n从0开始计算
		static size_t FREELIST_INDEX(size_t bytes) {
			return (((bytes)+EAligns::EAlign128 - 1) / EAligns::EAlign128 - 1);
		}
		//返回一个大小为n的对象，并可能加入大小为n的其他区块到free-list
		static void *refill(size_t n);
		//配置一大块空间，可容纳nobjs个大小为size的区块
		//如果配置nobjs个区块有所不便，nobjs可能会降低
		static char *chunk_alloc(size_t size, size_t& nobjs);
		static size_t get_blocks(size_t bytes);

	public:
		static void *allocate(size_t bytes);
		static void deallocate(void *ptr, size_t bytes);
		static void *reallocate(void *ptr, size_t old_sz, size_t new_sz);
	};

	//静态成员初始化
	char *alloc::start_free = nullptr;
	char *alloc::end_free = nullptr;
	size_t alloc::heap_size = 0;

	alloc::list_node *alloc::free_list[alloc::ENFreeLists::Free_list_num] = {
		nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,
	};
	// 根据大小获取区块数目
	inline size_t alloc::get_blocks(size_t bytes)
	{
		if (bytes <= 128)
		{
			return 8;
		}
		else if (bytes <= 256)
		{
			return 4;
		}
		else if (bytes <= 1024)
		{
			return 2;
		}
		else
		{
			return 1;
		}
	}

	//分配大小为bytes的空间
	void* alloc::allocate(size_t bytes) {
		if (bytes > static_cast<size_t>(EAligns::EAlign2K)) {
			return malloc(bytes);
		}
		size_t index = FREELIST_INDEX(bytes);
		list_node *list = free_list[index];
		if (list) {       //此list还有空间
			free_list[index] = list->next;
			return list;
		}
		else {//此list没有足够的空间，需要从内存池里面取空间
			return refill(ROUND_UP(bytes));
		}
	}
	//释放ptr指向的大小为bytes的空间，ptr不能为nullptr
	void alloc::deallocate(void *ptr, size_t bytes) {
		if (bytes > EAligns::EAlign2K) {
			free(ptr);
		}
		else {
			size_t index = FREELIST_INDEX(bytes);
			list_node *node = static_cast<list_node *>(ptr);
			node->next = free_list[index];
			free_list[index] = node;
		}
	}
	//重新分配ptr指向的空间，把old_sz大小的原空间分配为new_sz大小
	void* alloc::reallocate(void *ptr, size_t old_sz, size_t new_sz) {
		deallocate(ptr, old_sz);
		ptr = allocate(new_sz);
		return ptr;
	}
	//返回一个大小为n的对象，并且有时候会为适当的free list增加节点
	//假设bytes已经上调为8的倍数
	//重新填充free_list
	void* alloc::refill(size_t bytes) {
		size_t nobjs = get_blocks(bytes);
		//从内存池里取
		char *chunk = chunk_alloc(bytes, nobjs);
		list_node **my_free_list = nullptr;
		list_node *result = nullptr;
		list_node *current_obj = nullptr, *next_obj = nullptr;

		if (nobjs == 1) {//取出的空间只够一个对象使用
			return chunk;
		}
		else {
			my_free_list = free_list + FREELIST_INDEX(bytes);
			result = (list_node *)(chunk);
			*my_free_list = next_obj = (list_node *)(chunk + bytes);
			//将取出的多余的空间加入到相应的free list里面去
			for (int i = 1;; ++i) {
				current_obj = next_obj;
				next_obj = (list_node *)((char *)next_obj + bytes);
				if (nobjs - 1 == i) {
					current_obj->next = 0;
					break;
				}
				else {
					current_obj->next = next_obj;
				}
			}
			return result;
		}
	}

	//假设bytes已经上调为8的倍数
	//从内存池取出空间给free_list 
	char *alloc::chunk_alloc(size_t bytes, size_t& nobjs) {
		char *result = 0;
		size_t total_bytes = bytes * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes) {//内存池剩余空间完全满足需要
			result = start_free;
			start_free = start_free + total_bytes;
			return result;
		}
		else if (bytes_left >= bytes) {//内存池剩余空间不能完全满足需要，但足够供应一个或以上的区块
			nobjs = bytes_left / bytes;//调整nobjs大小
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {//内存池剩余空间连一个区块的大小都无法提供
			//申请heap空间
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			if (bytes_left > 0) {
				list_node **my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((list_node *)start_free)->next = *my_free_list;
				*my_free_list = (list_node *)start_free;
			}
			start_free = (char *)malloc(bytes_to_get);
			if (!start_free) {
				list_node **my_free_list = nullptr, *p = nullptr;
				for (int i = 0; i <= EAligns::EAlign2K; i += EAligns::EAlign128) {
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != 0) {
						*my_free_list = p->next;
						start_free = (char *)p;
						end_free = start_free + i;
						return chunk_alloc(bytes, nobjs);
					}
				}
				end_free = nullptr;
				std::cerr << "Out of memory !" << std::endl;
				std::exit(1);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(bytes, nobjs);
		}
	}
}
//namespace Tiny_STL .
#endif // !TINYSTL_ALLOC_H
