#pragma once
#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace test
{
	// Test 是一个抽象基类（用来定义“测试”的通用接口）。
	// virtual定义虚函数，之类可以选择是否重写
	// override 检查是否存在该虚函数
	// 纯虚函数才要强制重写
	class Test
	{
		public:
			Test() {}
			virtual ~Test() {}

			virtual void OnUpdate(float deltaTime) {}
			virtual void OnRender() {}
			virtual void OnImGuiRender() {}
	};


	class TestMenu: public Test
	{
		private:
			// 这是一个 引用的指针，表明testmenu不保存自己的指针
			Test*& m_CurrentTest; 

			// std::function<R(Args...)> 表示一个可调用对象（可以是函数、lambda、函数指针、仿函数）。
			// R 是返回类型 Args... 是参数列表
			// 这样点击菜单的时候，就可以通过调用.second创建一个实例化对象 
			std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;

		public:
			TestMenu(Test*& currentTestPtr);
			void OnImGuiRender() override;

			// 模板函数 vs 普通函数
			// 普通函数在编译时就确定了函数体，编译器可以在 .cpp 里编译好。
			// 模板函数直到实例化（用某个具体类型替换 T）时才真正生成代码。
			// 因此模板函数（和模板类的成员函数）一般要写在头文件里，这样编译器在看到调用的时候就能展开模板并生成对应代码。
			template<typename T>
			void RegisterTest(const std::string& name)
			{
				std::cout << "Register test: " << name << std::endl;

				// 是一个无捕获的 lambda，无参数，调用时在堆上 new 出一个 T 的实例并返回其指针（类型为 T*）。
				// [capture list] (parm list) -> return type {body};
				// 捕获
				m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
			}
		};
}