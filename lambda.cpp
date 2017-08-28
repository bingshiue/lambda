/**
 * @file lambda.cpp
 * @brief lambda expression test.
 */
/*
 ラムダ式の構文
 
 ラムダ式は以下に示す6つの部分で構成されています
 
  (1)  (2)  (3)      (4)      (5)
  ---  ---  -------  -------  -------
  [=]  ()   mutable  throw()  -> int
 |{
 |     int n = x + y;
 |     x = y;
 |     y = n;
(6)
 |
 |     return n;
 |     
 |}

 (1):lambda-introducer
  ラムダ式は[]から定義が始まります。[]の中には、キャプチャ宣言を記述します（後述）
  
 (2):lambda-parameter-declaration-list
  ラムダ式に与える引数の並びです。引数のデフォルト値は指定できませんし、可変長パラメータも不可。名前のない引数も許されません

 (3):mutable-specification
  ラムダ内にキャプチャ（後述）された変数の書き換え（代入/変更）を許すとき、「mutable」を指定します

 (4):exception-specification
  ラムダ式の中から例外をthrowするとき、「throw(送出される例外の並び)」を記述します
  
 (5):lambda-return-type-clause
  ラムダ式が返す値の型を->の後ろに指定します。ラムダ式の本体が1つのreturn式であるとき、またはラムダ式が値を返さないときは省略可能です
  [](int x) { std::cout << x; } // 省略可: void とみなす
  [](int x) { return x*2; }     // 省略可: intとみなす
  
 */
 
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

using namespace std;

/* template class for test "this" pointer in lambda capture parameter */
template<typename T>
class numbers {
	vector<T> data;
	void print_one(T x) const {cout << x << " ";}
public:
	void add(T x){ data.push_back(x);};
	void print_all() const {
		for_each(data.begin(), data.end(), [this](T x){print_one(x);});
	}
};

//#define USE_FUNCTION_OBJECT // Function Object (Traditional)

#ifndef USE_FUNCTION_OBJECT
#define USE_LAMBDA_EXPRESSION // Lambda Expression
#endif

#if defined(USE_FUNCTION_OBJECT) && defined(USE_LAMBDA_EXPRESSION)
#error "Both USE_FUNCTION_OBJECT & USE_LAMBDA_EXPRESSION Defined"
#endif

/* Function Object */
class can_divide { 
	int n_;
public:
	can_divide(int n) : n_(n) {}
	// If can divide, return true
	bool operator()(int x) const { return x % n_ == 0; }
};

/* Function Object */
class abs_less {
public:
	bool operator()(int x,int y) const {
		return ( x > 0 ? x : -x ) < ( y > 0 ? y : -y );
	}
};

int main(int argc,char** argv){
	
	int ar[] = {  9, 7, 5, 3, 1, -2, -4, -6, -8, 0 };
	int n = 4;

#ifdef USE_FUNCTION_OBJECT
	int* found = find_if(ar, ar+10, can_divide(n));
#endif

#ifdef USE_LAMBDA_EXPRESSION
	int* found = find_if(ar, ar+10, [n](int x){return x % n == 0;});
#endif
	
	
	if( found != ar+10 ){
		cout << *found << " can be divided by " << n << endl; 
	}
	
#ifdef USE_FUNCTION_OBJECT
	sort(ar, ar+10, abs_less());

	for(int idx=0; idx < 10; idx++){
		cout << ar[idx] << " ";
	}
	
#endif

#ifdef USE_LAMBDA_EXPRESSION
	sort(ar, ar+10, [](int x, int y){return ( x > 0 ? x : -x ) < ( y > 0 ? y : -y );});
	
	for_each(ar, ar+10, [](int x){ cout << x << " ";});
#endif

	cout << endl;

    /* ラムダ式を変数に */
    /*
	 ラムダ式は関数オブジェクトとして機能し、変数に格納できます。このときの変数の型には、C++0Xで新たに導入された型推論:auto、
	 もしくは標準ライブラリが提供するクラス・テンプレート:functionが使えます
     */
    // autoによる
    auto println = [](const std::string& str) { std::cout << str << std::endl; };

    // std::function<>による
    std::function<std::string (const std::string&)> polite = [](const std::string& str) { return str+" SIR "; };

    std::string msg = polite("Ben"); // politeの評価(呼び出し)
    println(msg); // printlnの評価(呼び出し)
    println(polite("Bingshiue"));
	
	/* キャプチャ（capture）とは */
	/*
	ラムダ式中で使えるオブジェクトは引数で受け渡された変数だけでなく、例えば上記のようにstd::coutのようなグローバルなインスタンスが使えます。

　  また、スコープ内のautomatic変数をラムダ式内部にキャプチャする（抱き込む）ことができます。この場合は[]内にキャプチャする変数名を指定します
	 */
	
	int number = 123;
	auto capt_print = [number]() {cout << number << " Captured " << endl;};
    n = 456; // ラムダ式の評価には影響を及ぼさない
    capt_print(); // "123 をキャプチャしています"
	
	cout << endl;
	
	/*
	ここで、試しにラムダ式の評価の度にnをインクリメントしてみましょう。キャプチャされた変数をラムダ式内部で変更するには、
	ラムダ引数の後にmutableを指定します
	 */
	
	int number2 = 123;
	auto capt_print2 = [number]() mutable {cout << number++ << " Captured " << endl;};
	capt_print2(); // "123 をキャプチャしています"
    capt_print2(); // "124 をキャプチャしています"
    capt_print2(); // "125 をキャプチャしています"
    std::cout << "number2 = " << number2 << std::endl;
	
	cout << endl;
	
	/*
	…おや？ キャプチャされた変数nは、ラムダ式の評価後に変更されていませんね。
	これは変数nの「値」がキャプチャされたからです（値キャプチャ）。
	ラムダ式の評価によってキャプチャされた変数を変更したいときは。「参照キャプチャ」を用います
	 */
	
	/* 参照キャプチャ */
	int number3 = 123;
    auto capt_print3 =
    //↓この'&'が参照キャプチャを意味する
    [&number3]() { std::cout << number3++ << " Captured \n"; };
    capt_print3(); // "123 をキャプチャしています"
    capt_print3(); // "124 をキャプチャしています"
    capt_print3(); // "125 をキャプチャしています"
    std::cout << "number3 = " << number3 << std::endl; // "number3 = 126"
    number3 = 456;
    capt_print3(); // "456 をキャプチャしています"
    std::cout << "number3 = " << number3 << std::endl; // "number3 = 457"
	
	cout << endl;
	
	/* ラムダ式に複数の変数をキャプチャするときは、それらを[]内に並べます */
	int target = 5;
    int count = 0;
    // targetより小さい要素の個数を数える
    auto count_less = [&count,target](int x) { if ( x < target) ++count; };

    int data[] = { 1, 3, 5, 7, 2, 4, 6, 8 };
    count = 0;
    std::for_each( data, data+8, count_less);
	std::cout << count << " Numbers " << "Less Than " << target << endl;
	
	cout << endl;
	
	/*
	'['の直後に'='を置くと「デフォルトで値キャプチャ」、'&'を置くと「デフォルトで参照キャプチャ」を意味します。
	従って、上記の[&count,target]は、[=,&count]あるいは[&,target]と書いてもかまいませんし、すべての変数が値キャプチャあるいは参照キャプチャであるなら、
	それぞれ[=]、[&]と略記できます
	 */
	 
	/* また、[]内にthisを指定することで、クラスのメンバ関数を呼び出すこともできます */
	numbers<int> ints;
	ints.add(1); ints.add(2); ints.add(3);
	ints.print_all();
	
	cout << endl;
	
	return 0;
}