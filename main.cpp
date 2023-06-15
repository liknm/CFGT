#include <iostream>
#include <set>
#include <vector>
#include <sstream>
using namespace std;
struct ProductionRule {
    char from{};
    set<string> to;
};
class ContextFreeGrammar {
    set<char> N;
    set<char> T;
    vector<ProductionRule> P;
    char S;
    static void readSymbolLine(set<char> &symbolList, istream &input) {
        string line;
        getline(input,line);
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
        output<<sign<<"={";
        bool first=true;
        for (const auto & s:symbolList) {
            if (first) {
                first= false;
            } else {
                output<<',';
            }
            output<<s;
        }
        cout<<'}'<<endl;
    }
    char readProductionRuleLineAndS(istream & input) {
        string line;
        while (getline(input,line)) {
            ProductionRule rule;
            if (line[0]=='\t') {
                rule.from=line[1];
                string symbols;
                istringstream iss(line.substr(5));
                while (getline(iss,symbols,'|')) {
                    rule.to.insert(symbols);
                }
                P.push_back(rule);
            } else {
                break;
            }
        }
        return line[2];
    }
    void printProductionRule(ostream & output=cout) {
        for (const auto & rule:P) {
            output<<'\t'<<rule.from<<"-->";
            bool first=true;
            for (const auto & s:rule.to) {
                if (first) {
                    first= false;
                } else {
                    output<<'|';
                }
                output<<s;
            }
            output<<endl;
        }
    }
public:
    explicit ContextFreeGrammar(istream &input = cin) {
        readSymbolLine(N, input);
        readSymbolLine(T, input);
        string buffer;
        getline(input,buffer);
        S=readProductionRuleLineAndS(input);
    }
    void print(ostream & output=cout) {
        printSymbolList(N,'N',output);
        printSymbolList(T,'T',output);
        cout<<"P:"<<endl;
        printProductionRule(output);
        cout<<"S="<<S<<endl;
    }
};
int main() {
    auto L = ContextFreeGrammar();
    L.print();
    return 0;
}
