#include <queue>
#include <string>
#include <stdlib.h>/*用到了srand函数，所以要有这个头文件*/
#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>
#include <sstream>
#include <time.h>

using namespace std;

class Promise {
private:
    bool ack;
    long ab;
    string av;

public:
    Promise(bool ack, long ab, string av) {
        this->ack = ack;
        this->ab = ab;
        this->av = av;
    }

    bool isAck() {
        return ack;
    }

    long getAb() {
        return ab;
    }

    string getAv() {
        return av;
    }
};
class Acceptor {

private:
    long   pb;
    long   ab;
    string av;
    string name;

public:
    Acceptor(string name) {
        this->name = name;
        this->pb = 0;
        this->ab = 0;
        this->av = "";
    }

    Promise* onPrepare(long b) {
        //假设这个过程有50%的几率失败
        if (rand() / double(RAND_MAX) - 0.5 > 0) {
            std::cout << "accepter: " << this->name << " prepare no response" << std::endl;
            return NULL;
        }

        if (b >= this->pb) {
            pb = b;
            Promise *response = new Promise(true, this->ab, this->av);
            std::cout << "accepter: " << this->name << " prepare ok" << std::endl;
            return response;
        } else {
            std::cout << "accepter: " << this->name << " prepare rejected" << std::endl;
            return new Promise(false, 0, "");
        }
    }

    bool onAccept(long b, string v) {
        //假设这个过程有50%的几率失败
        if (rand() / double(RAND_MAX) - 0.5 > 0) {
            std::cout << "accepter: " << this->name << " accept no response" << std::endl;
            return false;
        }

        if (b == this->pb) {
            ab = b;
            av = v;
            std::cout << "accepter: " << this->name << " accept ok < " << b << " : " << v << " >" << std::endl;
            return true;
        } else {
            std::cout << "accepter: " << this->name << " accept rejected" << std::endl;
            return false;
        }
    }

    long getPb() {
        return pb;
    }

    long getAb() {
        return ab;
    }

    string getAv() {
        return av;
    }

    string getName() {
        return name;
    }
};






string PROPOSALS[3] = {"Proposal_1", "Proposal_2", "Proposal_3"};

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

static void proposerVote(vector<Acceptor *> &acceptors) {
    int quorum = acceptors.size() / 2 + 1;
    int maxb = 0;
    int last_prepare_b = 0;
    int b = last_prepare_b + 1;
    int count_pre_ok = 0;
    int count_accept_ok = 0;
    int i = 0;
    queue<int> queue;
    string v;

    while (true) {
        v = PROPOSALS[rand() % ARRAY_LENGTH(PROPOSALS)];

        //得到b的序号
        if ((!queue.empty()) && rand() / double(RAND_MAX) - 0.5 > 0) {
            //乱序的序号到达
            b = queue.front();
            queue.pop();
        } else {
            b = last_prepare_b + 1;
            last_prepare_b ++;
            if (rand() / double(RAND_MAX) - 0.7 > 0) {
                //模拟乱序, 部分没有到达, 下一次随机到达
                queue.push(b);
                continue;
            }
        }
        std::cout << std::endl;
        std::cout << "**************************************************************" << std::endl;
        std::cout << "Accepter  pb  ab    av" << std::endl;
        for (i = 0; i < acceptors.size(); i ++) {
            std::cout << acceptors[i]->getName() << "         "
                      << acceptors[i]->getPb() << "   "
                      << acceptors[i]->getAb() << "   "
                      << acceptors[i]->getAv() << " "
                      << std::endl;
        }

        std::cout << std::endl;
        std::cout << "vote    : start < vote_number : " << b << " >" << std::endl;

        count_pre_ok = 0;
        for (i = 0; i < acceptors.size(); i ++) {
            Promise *promise = acceptors[i]->onPrepare(b);
            if (promise && promise->isAck()) {
                count_pre_ok ++;
                if (promise->getAb() > maxb && promise->getAv() != "") {
                    maxb = promise->getAb();
                    //使用maxvalue的v
                    v = promise->getAv();
                    std::cout << "vote    : v change < maxb : " << maxb << " v : " << v << " >" << std::endl;

                }
            }
        }

        if (count_pre_ok < quorum) {
            std::cout<<"prepare : end <" << b << "> : vote < not accepted >" << std::endl;
            continue;
        }

        count_accept_ok = 0;
        for (i = 0; i < acceptors.size(); i ++) {
            if (acceptors[i]->onAccept(b, v)) {
                count_accept_ok ++;
            }
        }

        if (count_accept_ok < quorum) {
            std::cout<<"accept  : end <" << b << ":" << v << "> : vote < not accepted >"<<std::endl;
            continue;
        }

        break;
    }
    std::cout<<"proposal: <" << b << ":" << v << "> : vote < success >"<<std::endl;
}

int main() {
    srand((unsigned)time(NULL));
    vector<Acceptor *> acceptors;
    acceptors.push_back(new Acceptor("A"));
    acceptors.push_back(new Acceptor("B"));
    acceptors.push_back(new Acceptor("C"));
    acceptors.push_back(new Acceptor("D"));
    acceptors.push_back(new Acceptor("E"));
    proposerVote(acceptors);
    return 0;
}
