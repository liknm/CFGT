#include <bits/stdc++.h>
using namespace std;
class ContextFreeGrammar {
    set<char> N;
    set<char> T;
    map<char, set<string>> P;
    char S;
    bool epsilon= false;
    static void readSymbolLine(set<char> &symbolList, istream &input) {
        string line;
        getline(input, line);
        auto start = line.find('{');
        auto end = line.find('}');
        istringstream ss(line.substr(start + 1, end - start - 1));
        char symbol;
        while (ss >> symbol) {
            if (symbol != ',') {
                symbolList.insert(symbol);
            }
        }
    }
    static void printSymbolList(const set<char> &symbolList, char sign, ostream &output = cout) {
        output << sign << "={";
        bool first = true;
        for (const auto &s: symbolList) {
            if (first) {
                first = false;
            } else {
                output << ',';
            }
            output << s;
        }
        cout << '}' << endl;
    }
    char readProductionRuleLineAndS(istream &input) {
        string line;
        while (getline(input, line)) {
            if (line[0] == '\t') {
                char from = line[1];
                string symbols;
                istringstream iss(line.substr(5));
                set<string> to;
                while (getline(iss, symbols, '|')) {
                    to.insert(symbols);
                }
                P.insert(pair<char, set<string>>(from, to));
            } else {
                break;
            }
        }
        return line[2];
    }
    void printProductionRule(ostream &output = cout) {
        for (const auto & C:N) {
            output << '\t' << C << "-->";
            bool first = true;
            for (const auto &s: P[C]) {
                if (! s.empty()) {
                    if (first) {
                        first = false;
                    } else {
                        output << '|';
                    }
                    output << s;
                }
            }
            output << endl;

        }
    }
    void findSymbolsTowards(const set<char> &terminal, set<char> &N1) {
        set<char> N0;
        set<char> Nd;
        do {
            N0 = Nd;
            set<char> unionSet;
            set_union(N0.begin(), N0.end(), terminal.begin(), terminal.end(), inserter(unionSet, unionSet.begin()));
            for (auto const &a: P) {
                for (auto const &t: a.second) {
                    if (terminalCheck(t, unionSet)) {
                        Nd.insert(a.first);
                    }
                }
            }
        } while (N0 != Nd);
        N1 = Nd;
    }
    static bool terminalCheck(const string &w, const set<char> &terminal) {
        return all_of(w.begin(), w.end(), [terminal](const char &c) {
            return (terminal.find(c) != terminal.end());
        });
    }
    void eliminateEpsilon() {
        set<char> Nd;
        findSymbolsTowards(set<char>{'N'}, Nd);
        map<char, set<string>> P1;
        for (auto &p: P) {
            for (auto const &q: p.second) {
                if (q != "N") {
                    generateCombinations(q, Nd, P1[p.first]);
                }
            }
        }
        if (Nd.find(S) != Nd.end()) {
            epsilon= true;
        }
        P = P1;
    }
    void eliminateUnitProduction() {
        map<char, set<char>> Ns;
        for (auto const &A: N) {
            set<char> N0 = {A};
            set<char> Nd;
            while (true) {
                set<char> Nc;
                for (auto const &B: N0) {
                    for (const auto &s: P[B]) {
                        if (s.length() == 1 && isupper(s[0])) {
                            Nc.insert(s[0]);
                        }
                    }
                }
                set_union(N0.begin(), N0.end(), Nc.begin(), Nc.end(), inserter(Nd, Nd.begin()));
                if (N0 != Nd) {
                    N0 = Nd;
                } else {
                    break;
                }
            }
            set<char> Na = Nd;
            Ns.insert(pair<char, set<char>>(A, Na));
        }
        map<char, set<string>> P1;
        for (auto const &p: P) {
            auto A = p.first;
            set<char> As;
            for (auto n: Ns) {
                if (n.second.find(A) != n.second.end()) {
                    As.insert(n.first);
                }
            }
            for (const auto &s: p.second) {
                if (s.length() > 1 || islower(s[0])) {
                    for (auto a: As) {
                        P1[a].insert(s);
                    }
                }
            }
        }
        P = P1;
    }

    static void generateCombinations(const std::string &s, const std::set<char> &terminal, std::set<std::string> &p) {
        std::queue<std::pair<std::string, int>> queue;
        queue.emplace("", 0);
        while (! queue.empty()) {
            std::string current = queue.front().first;
            int index = queue.front().second;
            queue.pop();
            if (index == s.length()) {
                p.insert(current);
                continue;
            }
            if (terminal.find(s[index]) == terminal.end()) {
                queue.emplace(current + s[index], index + 1);
            } else {
                queue.emplace(current, index + 1);
                queue.emplace(current + s[index], index + 1);
            }
        }
    }
    void findUsefulSymbols() {
        set<char> N0 = {S};
        set<char> Nd;
        while (true) {
            for (auto const &A: N0) {
                for (const auto &s: P[A]) {
                    for (auto c: s) {
                        Nd.insert(c);
                    }
                }
            }
            if (N0 == Nd) {
                break;
            }
            N0 = Nd;
        }
        set<char> N1;
        set<char> T1;
        set_intersection(Nd.begin(), Nd.end(), N.begin(), N.end(), inserter(N1, N1.begin()));
        set_intersection(Nd.begin(), Nd.end(), T.begin(), T.end(), inserter(T1, T1.begin()));
        N = N1;
        N.insert(S);
        T = T1;

    }
    void eliminateUselessSymbols() {
        findSymbolsTowards(T, N);
        findUsefulSymbols();
    }
    void dealWithEpsilon() {
        if (epsilon) {
            S='T';
            set<string> temp=P['S'];
            P.erase('S');
            temp.insert("N");
            P.insert(pair<char,set<string>>(S,temp));
            N.erase('S');
            N.insert('T');
        }
    }
public:
    explicit ContextFreeGrammar(istream &input = cin) {
        readSymbolLine(N, input);
        readSymbolLine(T, input);
        string buffer;
        getline(input, buffer);
        S = readProductionRuleLineAndS(input);
    }
    void print(ostream &output = cout) {
        printSymbolList(N, 'N', output);
        printSymbolList(T, 'T', output);
        cout << "P:" << endl;
        printProductionRule(output);
        cout << "S=" << S << endl;
    }
    void transform() {
        eliminateEpsilon();
        eliminateUnitProduction();
        eliminateUselessSymbols();
        dealWithEpsilon();
    }
};
int main() {
    auto L = ContextFreeGrammar();
    L.transform();
    L.print();
    return 0;
}
