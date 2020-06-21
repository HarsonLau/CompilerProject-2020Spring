#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include<sstream>
#include<regex>
#include <vector>
#include "IR.h"
#include "IRMutator.h"
#include "IRVisitor.h"
#include "IRPrinter.h"
#include "type.h"

using namespace Boost::Internal;

//去掉所有空格
void trim(std::string& s) {
	int index = 0;
	if (!s.empty()) {
		while ((index = s.find(' ', index)) != std::string::npos) {
			s.erase(index, 1);
		}
	}
}

//预处理json文件
void getInfo(std::string& ori, std::string& dest) {
	int pos1 = ori.rfind("\"");
	int pos2 = ori.rfind("\"", pos1 - 1);
	dest = ori.substr(pos2 + 1, pos1 - pos2 - 1);
}

void getMultiInfo(std::string& ori, std::vector<std::string>& dest) {
	int pos1 = ori.find("\"");
	int pos2 = ori.find("\"", pos1 + 1);
	pos1 = ori.find("\"", pos2 + 1);
	while (pos1 != int(std::string::npos)) {
		pos2 = ori.find("\"", pos1 + 1);
		dest.push_back(ori.substr(pos1 + 1, pos2 - pos1 - 1));
		pos1 = ori.find("\"", pos2 + 1);
	}
}

//将kernel中每个语句分开
void changeKernel(std::string& ori, std::vector<std::string>& dest) {
	int pos1 = 0;
	int pos2 = ori.find(";", pos1 + 1);
	while (pos2 != int(std::string::npos)) {
		std::string temp = ori.substr(pos1, pos2 - pos1);
		trim(temp);
		dest.push_back(temp);
		pos1 = pos2 + 2;
		pos2 = ori.find(";", pos1);
	}
}

//如果输入和输出有重复的，那么应该只出现在输入而不出现在输出
void changeOuts(std::vector<std::string>& ori, std::vector<std::string>& ins, std::vector<std::string>& dest) {

	for (auto it = ori.begin(); it != ori.end(); ++it) {
		auto it1 = std::find(ins.begin(), ins.end(), *it);
		if (it1 == ins.end())
			dest.push_back(*it);
	}
}

//表达式求值的方式，这边感觉是充分的，all里逐个放入了整个程序的变量，到时候kernel根据变量名找需要的
Expr E(std::string& s, bool isleft, std::vector<std::string>& left_index, Type& _data_type,
	std::vector<Expr>& all, std::vector<Expr>& index_list, std::vector<std::string>& cond);

Expr T(std::string& s, bool isleft, std::vector<std::string>& left_index, Type& _data_type,
	std::vector<Expr>& all, std::vector<Expr>& index_list, std::vector<std::string>& cond);

Expr F(std::string& s, bool isleft, std::vector<std::string>& left_index, Type& _data_type,
	std::vector<Expr>& all, std::vector<Expr>& index_list, std::vector<std::string>& cond);



//因为从左到右读index会多读，要去重。对于同名index，选则取值范围最小的那个
void insertIndex(std::vector<Expr>& index_list, Expr index) {
	bool flag = 1;
	std::vector<Expr> new_index_list;
	for (auto it = index_list.begin(); it != index_list.end(); ++it) {
		Ref<const Index>tempa = (*it).as<Index>();
		Ref<const Index>tempb = index.as<Index>();

        //判断是否同名
		if (tempa->name == tempb->name) {
			flag = 0;

			//选择域最小的那个index；由于没有析构函数，又不会使用mutator，只能用重新开个vector存一遍
			Expr temp_ori=tempa->dom, temp_new=tempb->dom;
			Ref<const Dom> dom_ori = temp_ori.as<Dom>();
			Ref<const Dom> dom_new = temp_new.as<Dom>();
			Expr range_ori=dom_ori->extent, range_new=dom_new->extent;
			Ref<const IntImm> int_ori = range_ori.as<IntImm>();
			Ref<const IntImm> int_new = range_new.as<IntImm>();
			if(int_ori->value()>int_new->value()){
				new_index_list.push_back(index);
			}
			else{
				new_index_list.push_back(*it);
			}
		}
		else{
			new_index_list.push_back(*it);
		}
	}
	if (flag)
		new_index_list.push_back(index);
	swap(new_index_list,index_list);
}

//和上面同理，从左到右读完的所有Expr值放在all中，根据json的信息找到需要的ins outs
void getInsOuts(std::vector<std::string>& _ins, std::vector<std::string>& _outs, std::vector<Expr>& ins,
	std::vector<Expr>& outs, std::vector<Expr>& all) {
	for (auto it = _ins.begin(); it != _ins.end(); ++it) {
		for (auto it1 = all.begin(); it1 != all.end(); ++it1) {
			Ref<const Var>tempa = (*it1).as<Var>();
			if (tempa->name == (*it)) {
				ins.push_back((*it1));
				break;
			}
		}
	}
	for (auto it = _outs.begin(); it != _outs.end(); ++it) {
		for (auto it1 = all.begin(); it1 != all.end(); ++it1) {
			Ref<const Var>tempa = (*it1).as<Var>();
			if (tempa->name == (*it)) {
				outs.push_back((*it1));
				break;
			}
		}
	}
}

/*处理Alist，输入s表示要处理的字符串，val：前面方括号里的变化范围，isleft：是否在等号左边
left_index：等号左边的下标，index_list：生成当前Var需要的下标，global_index_list：整个循环的下标
这部分非常偷鸡，只考虑了就最简单的组合，后续再加
如果发现下标是二元组合，如i+j，那么将其加入cond中用于if语句的构建，保证下标不越界
*/
void Alist(std::string& s, std::vector<size_t>& val, bool isleft, std::vector<std::string>& left_index,
	std::vector<Expr>& index_list, std::vector<Expr>& global_index_list, std::vector<std::string>& cond) {
	int size = val.size();
	int pos1 = 0;
	int pos2;
	for (int i = 0; i < size; ++i) {
		pos2 = s.find(',', pos1);
		std::string sub_s = s.substr(pos1, pos2 - pos1);
		pos1 = pos2 + 1;

		if (sub_s.find('*') != std::string::npos || sub_s.find('/') != std::string::npos
			|| sub_s.find('%') != std::string::npos) {
			std::regex reg("[a-z]+");
			std::smatch m;
			std::regex_search(sub_s, m, reg);
			Expr temp_dom = Dom::make(Type::int_scalar(32), 0, int(val[i]), m.str());

			//如果是在等号左边，加入left_index，否则就判断是否在等号左边出现过
			bool flag = 0;
			if (isleft)
				left_index.push_back(m.str());
			else {
				for (auto it = left_index.begin(); it != left_index.end(); ++it) {
					if (m.str() == (*it)) {
						flag = 1;
						break;
					}
				}
			}
			std::string target="/";
			 std::regex reg_temp("//");
			sub_s=regex_replace(sub_s,reg_temp,target);
			std::string temp_cond = '(' + sub_s + '<' + std::to_string(int(val[i])) + ')';
			cond.push_back(temp_cond);
			if (flag) {
				index_list.push_back(Index::make(Type::int_scalar(32), sub_s, temp_dom, IndexType::Spatial));
				insertIndex(global_index_list, Index::make(Type::int_scalar(32), m.str(), temp_dom, IndexType::Spatial));
			}
			else {
				index_list.push_back(Index::make(Type::int_scalar(32), sub_s, temp_dom, IndexType::Reduce));
				insertIndex(global_index_list, Index::make(Type::int_scalar(32), m.str(), temp_dom, IndexType::Reduce));
			}
		}
		else if (sub_s.find('+') != std::string::npos) {
			std::regex reg("[a-z]+");
			std::smatch m;
			std::string::const_iterator iterStart = sub_s.begin();
			std::string::const_iterator iterEnd = sub_s.end();
			std::vector<std::string> indexName;
			while (regex_search(iterStart, iterEnd, m, reg))
			{
				indexName.push_back(m[0]);
				iterStart = m[0].second;
			}
			if (indexName.size() == 1) {
				Expr temp_dom = Dom::make(Type::int_scalar(32), 0, int(val[i]), indexName[0]);

				bool flag = 0;
				if (isleft)
					left_index.push_back(m.str());
				else {
					for (auto it = left_index.begin(); it != left_index.end(); ++it) {
						if (m.str() == (*it)) {
							flag = 1;
							break;
						}
					}
				}
				std::string temp_cond = '(' + sub_s + '<' + std::to_string(int(val[i])) + ')';
				cond.push_back(temp_cond);
				if (flag) {
					index_list.push_back(Index::make(Type::int_scalar(32), sub_s, temp_dom, IndexType::Spatial));
					insertIndex(global_index_list, Index::make(Type::int_scalar(32), indexName[0], temp_dom, IndexType::Spatial));
				}
				else {
					index_list.push_back(Index::make(Type::int_scalar(32), sub_s, temp_dom, IndexType::Reduce));
					insertIndex(global_index_list, Index::make(Type::int_scalar(32), indexName[0], temp_dom, IndexType::Reduce));
				}

			}
			else {
				//使用binary的加
				Expr temp_dom = Dom::make(Type::int_scalar(32), 0, int(val[i]), indexName[0]);
				bool flag = 0;
				if (isleft)
					left_index.push_back(indexName[0]);
				else {
					for (auto it = left_index.begin(); it != left_index.end(); ++it) {
						if (indexName[0] == (*it)) {
							flag = 1;
							break;
						}
					}
				}

				if (flag)
					index_list.push_back(Index::make(Type::int_scalar(32), indexName[0], temp_dom, IndexType::Spatial));
				else
					index_list.push_back(Index::make(Type::int_scalar(32), indexName[0], temp_dom, IndexType::Reduce));
				insertIndex(global_index_list, index_list.back());

				//这边只考虑i+j ， 不考虑i+j+k or i+j+1，可能还要处理
				Expr temp_dom1 = Dom::make(Type::int_scalar(32), 0, int(val[i]), indexName[1]);

				flag = 0;
				if (isleft)
					left_index.push_back(indexName[1]);
				else {
					for (auto it = left_index.begin(); it != left_index.end(); ++it) {
						if (indexName[1] == (*it)) {
							flag = 1;
							break;
						}
					}
				}
				std::string temp_cond = '(' + indexName[0] + '+' + indexName[1] + '<' + std::to_string(int(val[i])) + ')';
				cond.push_back(temp_cond);
				if (flag)
					index_list.push_back(Index::make(Type::int_scalar(32), indexName[1], temp_dom1, IndexType::Spatial));
				else
					index_list.push_back(Index::make(Type::int_scalar(32), indexName[1], temp_dom1, IndexType::Reduce));
				insertIndex(global_index_list, index_list.back());

				Expr right = index_list.back();
				index_list.pop_back();
				Expr left = index_list.back();
				index_list.pop_back();
				Expr bin = Binary::make(Type::int_scalar(32), BinaryOpType::Add, left, right);
				index_list.push_back(bin);
			}
		}
		else {
			std::regex reg("[a-z]+");
			std::smatch m;
			std::regex_search(sub_s, m, reg);
			Expr temp_dom = Dom::make(Type::int_scalar(32), 0, int(val[i]), m.str());
			bool flag = 0;
			if (isleft)
				left_index.push_back(m.str());
			else {
				for (auto it = left_index.begin(); it != left_index.end(); ++it) {
					if (m.str() == (*it)) {
						flag = 1;
						break;
					}
				}
			}

			if (flag)
				index_list.push_back(Index::make(Type::int_scalar(32), m.str(), temp_dom, IndexType::Spatial));
			else
				index_list.push_back(Index::make(Type::int_scalar(32), m.str(), temp_dom, IndexType::Reduce));
			insertIndex(global_index_list, index_list.back());
		}
	}
}

//处理Tref，这里只读入[]内的数据范围，剩下的给Alist去处理
Expr TRef(std::string& s, bool isleft, std::vector<std::string>& left_index, Type& _data_type,
	std::vector<Expr>& all, std::vector<Expr>& index_list, std::vector<std::string>& cond) {
	std::vector<Expr>local_index_list;
	std::vector<size_t> val;
	int pos1 = s.find("<");
	int pos2 = s.find(">", pos1 + 1);
	int pos3 = s.find("[");
	int pos4 = s.find("]", pos3 + 1);

	//读取[]内的值
	std::string sub_s = s.substr(pos1 + 1, pos2 - pos1 - 1);
	std::istringstream is(sub_s);
	int inter;
	char ch;
	while (is >> inter){
		val.push_back(inter);
		is >> ch;
	}

	//处理<>内的值
	sub_s = s.substr(pos3 + 1, pos4 - pos3 - 1);
	sub_s += ',';
	Alist(sub_s, val, isleft, left_index, local_index_list, index_list, cond);

	//加上id的名称即可构建Var
	std::regex reg("[a-zA-Z]+");
	std::smatch m;
	std::regex_search(s, m, reg);
	Expr expr_temp = Var::make(_data_type, m.str(), local_index_list, val);
	all.push_back(expr_temp);
	return expr_temp;
}

/*表达式求值的递归
Expr->Term+Expr
	||Term-Expr
Term->Factor*Term
	||Factor/Term
	||Factor%Term
	||Factor//Term
Factor->id||(Expr)
*/
Expr E(std::string& s, bool isleft, std::vector<std::string>& left_index, Type& _data_type,
	std::vector<Expr>& all, std::vector<Expr>& index_list, std::vector<std::string>& cond) {

	std::vector<Expr>expr_left;//这边不用队列逐个存下来的话，后面Binary::make会报错，不是很懂
	expr_left.push_back(T(s, 0, left_index, _data_type, all, index_list, cond));
	bool more = true;
	while (more&&(!s.empty())) {
		if (s[0] == '+') {
			s.erase(0, 1);//去掉+
			Expr expr_right = T(s, 0, left_index, _data_type, all, index_list, cond);
			//直接写expr_left=Binary::make(_data_type, BinaryOpType::Add, expr_left, expr_right);会报错
			expr_left.push_back(Binary::make(_data_type, BinaryOpType::Add, expr_left.back(), expr_right));
		}
		else if (s[0] == '-') {
			s.erase(0, 1);
			Expr expr_right = T(s, 0, left_index, _data_type, all, index_list, cond);
			expr_left.push_back(Binary::make(_data_type, BinaryOpType::Sub, expr_left.back(), expr_right));
		}
		else
			more = false;
	}
	return expr_left.back();
}

Expr T(std::string& s, bool isleft, std::vector<std::string>& left_index, Type& _data_type,
	std::vector<Expr>& all, std::vector<Expr>& index_list, std::vector<std::string>& cond) {

	std::vector<Expr>expr_left;
	expr_left.push_back(F(s, 0, left_index, _data_type, all, index_list, cond));
	bool more = true;
	while (more && (!s.empty())) {

		if (s[0] == '*') {
			s.erase(0, 1);
			Expr expr_right = F(s, 0, left_index, _data_type, all, index_list, cond);
			expr_left.push_back(Binary::make(_data_type, BinaryOpType::Mul, expr_left.back(), expr_right));

		}
		else if (s[0] == '/') {
			s.erase(0, 1);
			if (s[0] == '/')
				s.erase(0, 1);
			Expr expr_right = F(s, 0, left_index, _data_type, all, index_list, cond);
			expr_left.push_back(Binary::make(_data_type, BinaryOpType::Div, expr_left.back(), expr_right));
		}
		else if (s[0] == '%') {
			s.erase(0, 1);
			Expr expr_right = F(s, 0, left_index, _data_type, all, index_list, cond);
			expr_left.push_back(Binary::make(_data_type, BinaryOpType::Mod, expr_left.back(), expr_right));
		}
		else
			more = false;
	}
	return expr_left.back();
}

Expr F(std::string& s, bool isleft, std::vector<std::string>& left_index, Type& _data_type,
	std::vector<Expr>& all, std::vector<Expr>& index_list, std::vector<std::string>& cond) {

	if (s[0] == '(') {
		s.erase(0, 1);
		Expr expr_left = E(s, 0, left_index, _data_type, all, index_list, cond);
		s.erase(0, 1);//去掉 ）
		return expr_left;
	}
	else {
		std::regex reg("^[a-zA-Z]+<[0-9,]+>\\[[a-z,\\+\\*%/0-9]+\\]");
		std::regex reg1("^[a-zA-Z]+<[0-9,]+>");
		std::smatch m, m1;
		if (std::regex_search(s, m, reg)) {
			std::string str_temp = m.str();
			Expr temp = TRef(str_temp, 1, left_index, _data_type, all, index_list, cond);
			s = m.suffix();
			return temp;
		}
		else if (std::regex_search(s, m, reg1)) {
			std::regex reg2("^[a-zA-Z]+");
			std::regex_search(s, m1, reg2);
			Expr temp = Var::make(_data_type, m1.str(), {}, {});
			s = m.suffix();
			all.push_back(temp);
			return temp;
		}
		else {
			std::regex reg2("^[0-9\\.]+");
			std::regex_search(s, m, reg2);
			Expr temp = Var::make(_data_type, m.str(), {}, {});
			s = m.suffix();
			return temp;
		}
	}
}


//等号左边的丢给Tref处理，右边的给E去处理，最后合一下
void formMainBodyList(std::string& s, Type& _data_type, std::vector<Expr>& all,
	std::vector<Expr>& index_list, std::vector<Stmt>& body_list) {
	std::vector<std::string> cond; //记录复合下标，防止越界
	std::vector<std::string> left_index; //记录左侧的那些下标

	//处理等号左侧
	int pos1 = 0, pos2 = s.find("=", pos1);
	std::string sub_s = s.substr(pos1, pos2 - pos1 - 1);
	Expr expr_left = TRef(sub_s, 1, left_index, _data_type, all, index_list, cond);

    //处理等号右侧
	pos1 = pos2 + 1;
	pos2 = s.find(";", pos1 + 1);
	sub_s = s.substr(pos1, pos2 - pos1);
	Expr expr_right = E(sub_s, 0, left_index, _data_type, all, index_list, cond);

	//对所有下标的条件组合
	std::string condtion;
	for (auto it = cond.begin(); it != cond.end(); ++it) {
		if (!condtion.empty()) {
			condtion = condtion + "&&";
		}
		condtion = condtion + (*it);
	}

	//构建move语句
	Stmt move = Move::make(expr_left, expr_right, MoveType::MemToMem);

	//构建if语句，这里没用标准的condtion节点，因为之前存的是string类型的condtion，而标量节点的唯一属性是name，
	//这里只需用这个string构造标量节点Var即可满足需求
	if (cond.empty())
		body_list.push_back(move);
	else {
		Expr cond_cond = Var::make(Type::int_scalar(32), condtion, {}, {});
		Stmt real_cond = IfThenElse::make(cond_cond, move, {});
		body_list.push_back(real_cond);
	}
}

//循环，
Stmt formLoop(std::string& s, Type& _data_type, std::vector<Expr>& all) {

	std::vector<Expr> index_list;//index_list里面存放了读取过程中碰到的index，插入新的时需要去重
	std::vector<Stmt> body_list;
	//index_list, body_list交给后面去生成
	formMainBodyList(s, _data_type, all, index_list, body_list);

	Stmt loop_nest = LoopNest::make(index_list, body_list);
	return loop_nest;
}


std::string formIR(std::string& _name, std::string& _data_type, std::vector<std::string>& _ins,
	std::vector<std::string>& _outs, std::vector<std::string>& _kernel) {
	Type data_type = (_data_type == "float") ? Type::float_scalar(32) : Type::int_scalar(32);
	std::vector<Expr> ins, outs, all;//all里面存放了表达式中出现的所有ABC变量，需要去重
	std::vector<Stmt> stmt_list;

	//将每一句语句加入stmt_list
	for (auto it = _kernel.begin(); it != _kernel.end(); ++it) {
		stmt_list.push_back(formLoop(*it, data_type, all));
	}

	//获取所需的ins，outs以生成签名
	getInsOuts(_ins, _outs, ins, outs, all);
	Group kernel = Kernel::make(_data_type, _name, ins, outs, stmt_list);

	//这些是直接复制过来的，也不清楚具体有什么用
	// visitor
	IRVisitor visitor;
	kernel.visit_group(&visitor);

	// mutator
	IRMutator mutator;
	kernel = mutator.mutate(kernel);

	// printer
	IRPrinter printer;
	std::string code = printer.print(kernel);
	return code;
}



void Routine(const std::string case_name){
	std::ifstream myfile("./cases/"+case_name+".json");
	if(myfile.fail()){
		std::cout<<case_name<<" not exist"<<std::endl;
		return;
		}
	std::string temp, name, data_type, temp_kernel;
	std::vector<std::string> temp_outs, ins, outs, kernel;
	getline(myfile, temp);
	getline(myfile, temp);
	getInfo(temp, name);
	getline(myfile, temp);
	getMultiInfo(temp, ins);
	getline(myfile, temp);
	getMultiInfo(temp, temp_outs);
	changeOuts(temp_outs, ins, outs);
	getline(myfile, temp);
	getInfo(temp, data_type);
	getline(myfile, temp);
	getInfo(temp, temp_kernel);
	getline(myfile, temp);
	myfile.close();
	changeKernel(temp_kernel, kernel);


	std::string program = "#include \"../run.h\"\n";
	program += "//"+temp_kernel+"\n\n";
	program += formIR(name, data_type, ins, outs, kernel);
	std::ofstream ofile("./kernels/kernel_"+case_name+".cc", std::ios::out);
	ofile << program;
	ofile.close();
	std::cout<<case_name<<" finished"<<std::endl;
}

int main() {
	// Input is the file name without suffix(.json)
	Routine("example");
	Routine("case1");
	Routine("case2");
	Routine("case3");
	Routine("case4");
	Routine("case5");
	Routine("case6");
	Routine("case7");
	Routine("case8");
	Routine("case9");
	Routine("case10");

	return 0;
}


