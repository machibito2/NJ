#include <iostream>
#include <vector>
#include <sstream>
#include <map>
using namespace std;
const double INF = (double)((1<<31)-1);

class Tree
{
private:
	string print()
	{
		stringstream ss;
		if (childs.size() == 0)
		{
			ss << name << ":" << score;
		}
		else
		{
			ss << "(";
			for (int i = 0; i < childs.size() - 1; ++i)
			{
				ss << childs[i].print() << ",";
			}
			ss << childs[childs.size()-1].print() << ")" << name;
			if (score != 0)
			{
				ss << ":" << score;
			}
		}
		return ss.str();
	}
	void erase(string n)
	{
		for (auto it = childs.begin(); it != childs.end();)
		{
			if (it->name == n)
			{
				it = childs.erase(it);
			}
			else
			{
				++it;;
			}
		}
	}
	Tree search(string n)
	{
		Tree retData = Tree();
		for (auto it = childs.begin(); it != childs.end(); ++it)
		{
			if (it->name == n)
			{
				retData = *it;
				break;
			}
		}
		return retData;
	}
public:
	Tree()
	{
		name = "root";
		score = INF;
	}
	Tree(string n)
	{
		name = n;
		score = INF;
	}
	Tree (string n, double m)
	{
		name = n;
		score = m;
	}
	~Tree(){}
	void insert(Tree child)
	{
		childs.push_back(child);
	}
	void insert(string s, double n)
	{
		Tree in(s, n);
		insert(in);
	}
	string rootPrint()
	{
		stringstream ss;
		ss << "(";
		for (int i = 0; i < childs.size() - 1; ++i)
		{
			ss << childs[i].print() << ",";
		}
		ss << childs[childs.size()-1].print() << ")" << name << ";";
		return ss.str();
	}
	string name;
	double score;
	vector<Tree> childs;
};

class TreeManager
{
private:
	void insertTree(string s, double score)
	{
		allTree[s] = Tree(s, score);
	}
public:
	TreeManager()
	{
		root = Tree("root", 0);
	}
	~TreeManager(){}
	void mergeTree(string ls, double ln, string rs, double rn, string roots)
	{
		Tree left, right;
		if (allTree.find(ls) == allTree.end()) // Not found
		{
			left = Tree(ls, ln);
		}
		else
		{
			left = allTree[ls];
			left.score = ln;
			allTree.erase(ls);
		}

		if (allTree.find(rs) == allTree.end()) // Not found
		{
			right = Tree(rs, rn);
		}
		else
		{
			right = allTree[rs];
			right.score = rn;
			allTree.erase(rs);
		}
		if (ls == roots)
		{
			left.insert(right);
			allTree[roots] = left;
		}
		else if (rs == roots)
		{
			right.insert(left);
			allTree[roots] = right;
		}
		else
		{
			Tree mergeRoot(roots, 0);
			mergeRoot.insert(left);
			mergeRoot.insert(right);
			allTree[roots] = mergeRoot;
		}
	}
	string result()
	{
		Tree rr("root", 0);
		for (auto it = allTree.begin(); it != allTree.end(); ++it)
		{
			rr.insert(it->second);
		}
		return rr.rootPrint();
	}
	Tree root;
	map<string, Tree> allTree;
	/* data */
};

int main ()
{
	TreeManager tm;
	tm.mergeTree("A", 0.3, "B", 0.4, "E");
	tm.mergeTree("C", 0.2, "D", 0.5, "F");
	tm.mergeTree("E", 0, "F", 0.1, "F");
	cout << tm.result() << endl;
}
