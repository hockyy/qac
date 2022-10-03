#include <bits/stdc++.h>
using namespace std;

namespace Color {
enum Code {
  FG_RED     = 31,
  FG_GREEN   = 32,
  FG_YELLOW  = 33,
  FG_BLUE    = 34,
  FG_MAGENTA = 35,
  FG_CYAN    = 36,
  FG_DEFAULT = 39
};
class ColorDebugger {
  Code code;
 public:
  ColorDebugger(Code pCode) : code(pCode) {}
  template <class T>
  ColorDebugger& operator<<(const T &other) {
    cout << other;
    return *this;
  }
  ColorDebugger& operator<<(ostream & (*pManip)(ostream&)) {
    (*pManip)(cout);
    return *this;
  }
};
}

using namespace Color;
ColorDebugger rcout(FG_RED);
ColorDebugger ycout(FG_YELLOW);
ColorDebugger gcout(FG_GREEN);
ColorDebugger bcout(FG_BLUE);
ColorDebugger mcout(FG_MAGENTA);
ColorDebugger ccout(FG_CYAN);


typedef pair<string, int> FrequencyPair;
typedef pair<int, int> Range;
typedef pair<int, Range> FreqRangePair;

const int NOT_INIT = -1;
const int ROOT = 0;

// Assuming string is pairwise distinct,
// Jika tidak, bisa gunakan ending terminal node untuk leaf
struct TrieNode {

  int freq = 0;
  char character = '$';
  Range range = {NOT_INIT, NOT_INIT};
  map<char, int> children = map<char, int>();

  TrieNode(char _character = '$'): character(_character) { }
};

struct Trie {

  int N;
  vector <TrieNode> trieNodes;
  vector <int> frequencyTable;
  vector <FrequencyPair> stringData;
  vector <vector <int>> rmqTable;

  Trie (vector <FrequencyPair> _stringData) : stringData(_stringData) {
    N = stringData.size();
    sort(begin(stringData), end(stringData));
    trieNodes.push_back(TrieNode());

    ycout << "Creating Trie:" << endl;
    for (auto &freqPair : stringData) {

      int pos = ROOT;
      trieNodes[pos].freq += freqPair.second;

      for (auto &ch : freqPair.first) {
        pos = getNextNode(ch, pos);
        // cout << pos << " " << freqPair.second << endl;
        trieNodes[pos].freq += freqPair.second;
      }
    }

    gcout << "DFS Childrens:" << endl;
    // Initialize frequency table by depth first search
    dfs(ROOT);
    bcout << "Frequency Table:" << endl;
    for (int i = 0; i < N; i++) bcout << frequencyTable[i] << " \n"[i == N - 1];

    rmqTable.resize(N, vector<int>(N));
    for (int i = 0; i < N; i++) {
      int currentMaximum = i;
      for (int j = i; j < N; j++) {
        if (frequencyTable[j] > frequencyTable[currentMaximum]) {
          currentMaximum = j;
        }
        rmqTable[i][j] = currentMaximum;
      }
    }
    rcout << "RMQ Table:" << endl;
    for(int i = 0;i < N;i++){
      for(int j = 0;j < N;j++){
        rcout << rmqTable[i][j] << " ";
      }
      rcout << endl;
    }
  }

  /**
   * Mengembalikan node selanjutnya yang mengarah ke ch dari trieNodes[pos]
   * Bila tidak ada, buat yang baru
   **/
  int getNextNode(char ch, int pos) {
    if (!trieNodes[pos].children.count(ch)) {
      int newIndex = trieNodes.size();
      ycout << "Membuat node dari " << trieNodes[pos].character << " - " << pos << " ke: " << ch << " - " << newIndex << endl;
      trieNodes.push_back(TrieNode(ch));
      trieNodes[pos].children[ch] = newIndex;
      return newIndex;
    }
    return trieNodes[pos].children[ch];
  }

  // Untuk menginisialisasi frequency table
  void dfs(int pos) {
    bool isChildren = (trieNodes[pos].children.size() == 0);
    if (isChildren) {
      gcout << "Children " << pos << " " << trieNodes[pos].freq << endl;
      int currentIndex = frequencyTable.size();
      trieNodes[pos].range = {currentIndex, currentIndex};
      frequencyTable.push_back(trieNodes[pos].freq);
      return;
    }
    // Traverse this node to its children
    for (auto &nx : trieNodes[pos].children) {
      // nx.first adalah karakternya
      // nx.second adalah posnya
      dfs(nx.second);
      if (trieNodes[pos].range.first == NOT_INIT) {
        trieNodes[pos].range.first = trieNodes[nx.second].range.first;
      }
      trieNodes[pos].range.second = trieNodes[nx.second].range.second;
    }
  }

  vector<string> query (string queryString, int K) {
    vector <string> answer;
    priority_queue<FreqRangePair> heap;

    bool ok = 1;
    int pos = ROOT;
    // Traverse
    for (char &ch : queryString) {
      if (!trieNodes[pos].children.count(ch)) {
        ok = 0;
        break;
      }
      pos = trieNodes[pos].children[ch];
    }

    if (!ok) return {};

    Range answerRange = trieNodes[pos].range;
    // FreqRangePair:
    // - First : frequency
    // - Second : pair range

    mcout << "Dapat range:" << endl;
    mcout << answerRange.first << " " << answerRange.second << endl;
    mcout << "Frequency debug: " << endl;
    for(int i = answerRange.first;i <= answerRange.second;i++){
      mcout << frequencyTable[i] << " ";
    }
    mcout << endl;

    heap.push( {
      rmqTable[answerRange.first][answerRange.second],
      answerRange
    });

    K = min(K, answerRange.second - answerRange.first + 1);

    int iter = 0;
    while (answer.size() < K) {
      FreqRangePair best = heap.top();
      heap.pop();
      answer.push_back(stringData[best.first].first);
      int kiri = best.second.first;
      int kanan = best.second.second;
      int bestPosition = rmqTable[kiri][kanan];
      iter++;
      ycout << endl;
      ycout << "Iterasi ke-" << iter << ":" << endl;
      ycout << "range: " << best.second.first << " " << best.second.second << endl;
      ycout << "index best: " << bestPosition << endl;
      ycout << "index value: " << best.first << endl;
      ycout << "Splitting ..." << endl;
      // Split kiri
      if (kiri <= bestPosition - 1) {
        ycout << "push " << kiri << " " << bestPosition - 1 << endl;
        heap.push({
          rmqTable[kiri][bestPosition - 1],
          {kiri, bestPosition - 1}
        });
      }
      // Split kanan
      if (bestPosition + 1 <= kanan) {
        ycout << "push " << bestPosition + 1 << " " << kanan << endl;
        heap.push({
          rmqTable[bestPosition + 1][kanan],
          {bestPosition + 1, kanan}
        });
      }
    }
    return answer;
  }
};

int main() {
  vector <FrequencyPair> stringData = {
    {"nba", 6},
    {"news", 6},
    {"nab", 8},
    {"ngv", 9},
    {"netflix", 7},
    {"netbank", 8},
    {"network", 1},
    {"netball", 3},
    {"netbeans", 4}
  };
  Trie trie(stringData);
  string QUERY_STRING = "net";
  int K = 3;
  auto result = trie.query(QUERY_STRING, K);
  cout << "Hasil query \"" << QUERY_STRING << "\" - " << K << endl;
  for(auto &str : result){
    cout << str << endl;
  }
}