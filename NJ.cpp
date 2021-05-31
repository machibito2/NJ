#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
using namespace std;
const double INF = (double)((1<<31)-1);

// 木自体のクラス
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
		for (int i = 0; i < childs.size() - 1; ++i)
		{
			ss << childs[i].print() << ",";
		}
		ss << childs[childs.size()-1].print() << ";";
		return ss.str();
	}
	string name;
	double score;
	vector<Tree> childs;
};

// 全ての木を管理するクラス
// 基本こっちしか触らない
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
};

// CSVのカンマを区切る
vector<string> split(string& input, char delimiter)
{
	istringstream stream(input);
	string field;
	vector<string> result;
	while (getline(stream, field, delimiter))
	{
		if (field == "")
		{
			continue;
		}
		result.push_back(field);
	}
	return result;
}

// r_{ij}の計算
vector<double> calcR(vector<vector<double> > &data)
{
	int size = data[0].size();
	vector<double> retData(size);
	for (int i = 0; i < size; ++i)
	{
		retData[i] = 0;
		for (int j = 0; j < size; ++j)
		{
			retData[i] += data[i][j];
		}
		retData[i] /= (size - 2);
	}
	return retData;
}

// D_{ij}の計算
pair<int, int> calcD(vector<double> r, vector<vector<double> > data)
{
	int size = r.size();
	// 最も小さいDはどれを選んでもいいからmapでいい
	map<double, pair<int, int> > allD;
	double nowD;
	for (int i = 0; i < size; ++i)
	{
		for (int j = i+1; j < size; ++j)
		{
			nowD = data[i][j] - (r[i] + r[j]);
			allD[nowD] = make_pair(i, j);
		}
	}
	return allD.begin()->second;
}

// 2つをくっつけたノードを追加した後のd_{ij}の計算
vector<vector<double> > calcNextData(vector<double> r, vector<vector<double> >& data, pair<int, int> merge, map<int, string> numberToName)
{
	int nextSize = data[0].size()-1;	// 次のデータは2個引かれて1個足すから-1
	int icount = 0, jcount = 0;
	vector <vector<double> > resData(nextSize, vector<double> (nextSize));
	icount = 0;

	for (int i = 0; i < nextSize + 1; ++i)
	{
		jcount = 0;
		if (i == merge.first || i == merge.second)
		{
			continue;
		}
		for (int j = 0; j < nextSize + 1; ++j)
		{
			if (j == merge.first || j == merge.second)
			{
				continue;
			}
			resData[icount][jcount++] = data[i][j];
		}
		++icount;
	}

	icount = 0;
	for (int i = 0; i < nextSize + 1; ++i)
	{
		if (i == merge.first || i == merge.second)
		{
			continue;
		}
		resData[icount][nextSize - 1] = resData[nextSize - 1][icount] = (data[merge.first][i] + data[merge.second][i] - data[merge.first][merge.second])/2;
		++icount;
	}
	return resData;
}

void input(vector <vector<double> > &data, map <int, string> &numberToName, ifstream &ifs)
{
	string line = "";

	int size = 0;

	getline (ifs, line);
	vector<string> strvec = split (line, ',');
	size = strvec.size ();
	string dai = "";
	for (int i = 0; i < strvec[strvec.size()-1].size()-1; ++i)	// CSVから読み取るときに最後の要素に謎のNULL文字らしきものが1文字目だけにつくから
	{
		dai += strvec[strvec.size()-1][i];
	}
	strvec[strvec.size()-1] = dai;
	for (int i = 0; i < size;i++)
	{
		numberToName[i] = strvec[i];
	}

	// 読み捨て
	getline (ifs, line);

	// 数値を読む
	data.resize(size, vector<double>(size));

	for (int i = 1; i < size; ++i)
	{
		getline(ifs, line);
		vector<string> strvec = split (line, ',');
		for (int j = 1; j < strvec.size () - 1; ++j)
		{
			data[i][j-1] = data[j-1][i] = stof(strvec[j]);
		}
	}
}

void NeighborJoining(vector <vector<double> > &data, map <int, string> &numberToName, TreeManager &tm)
{
	int nameCounter = 0;

	int nokori = data[0].size();

	while (nokori > 2)
	{
		int nowsize = data[0].size();
		stringstream ss;
		vector<double> r = calcR(data);

		pair<int, int> dres = calcD(r, data);
		int left = dres.first;	// leftの番号
		int right = dres.second;	// rightの番号
		string leftname = numberToName[left];	// leftの名前
		string rightname = numberToName[right];	// rightの名前
		// 名前と番号のリストをずらす
		int icount = 0;
		for (int i = 0; i < nowsize; ++i)
		{
			if (i == left || i == right)
			{
				continue;
			}
			numberToName[icount] = numberToName[i];
			++icount;
		}

		ss << "node" << nameCounter++;
		numberToName[nowsize-2] = ss.str();
		for (int i = nowsize - 1; i <= nowsize + icount; ++i)
		{
			numberToName.erase(i);	// 2個を消す
		}

		nokori = numberToName.size();

		vector <vector<double> > nextData = calcNextData(r, data, dres, numberToName);
		double leftd = (data[left][right]+r[left]-r[right])/2;
		double rightd = (data[left][right] - leftd);
		tm.mergeTree(leftname, leftd, rightname, rightd, ss.str());
		data.clear();
		data.resize(nextData.size(), vector<double>(nextData.size()));
		data = nextData;	// 今回計算した値を次の入力にする
	}
	vector<double> r = calcR(data);
	pair<int, int> dres = calcD(r, data);
	// cout << numberToName[0] << endl;
	// cout << numberToName[1] << endl;
	if (numberToName[0].find("node") != string::npos)	// found
	{
		if (numberToName[1].find("node") != string::npos)	//found
		{
			tm.mergeTree(numberToName[0], data[dres.first][dres.second], numberToName[1], data[dres.first][dres.second], "end");
		}
		else	// not found
		{
			tm.mergeTree(numberToName[0], 0, numberToName[1], data[dres.first][dres.second], numberToName[0]);
		}
	}
	else	//not found
	{
		tm.mergeTree(numberToName[0], data[dres.first][dres.second], numberToName[1], 0, numberToName[1]);
	}
}

void owari(TreeManager &tm)
{
	string ans = tm.result();
	ofstream outputfile("net.nwk");
	outputfile << ans;
	outputfile.close();
}

int main()
{
	ifstream ifs("sensei.csv");
	map<int, string> numberToName;
	vector<vector<double> > data;
	input(data, numberToName, ifs);
	TreeManager tm;
	NeighborJoining(data, numberToName, tm);
	owari(tm);
}


