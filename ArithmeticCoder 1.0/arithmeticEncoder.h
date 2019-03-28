//
// Created by Freddy Don(董昊 2016210772) on 2019/3/27.
//

#ifndef ARITHMETICCODER_H

#define ARITHMETICCODER_H

#include <list>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstring>

using namespace std;

class arithmeticCoder {
private:
    uint32_t p[256];//每个ASCII字符的概率
    uint32_t P[256];//累计概率
    string str;//被压缩的字符串
    bool Encoded, Decoded;//标记是否被编码/解码
    uint32_t *code;//指向编码之后产生的数据

    class bigBinaryDecimal {
    private:
        list<uint32_t> arr;
    public:
        bigBinaryDecimal() {
        }

        bigBinaryDecimal(const uint32_t i) {
            arr.push_back(i);
        }

        bigBinaryDecimal(const uint32_t *i, size_t len) {
            while (len--) {
                this->arr.push_back(*(i++));
            }
        }

        bigBinaryDecimal(bigBinaryDecimal &i) {
            arr = i.arr;
        }

        bigBinaryDecimal(bigBinaryDecimal &&i) {
            arr = move(i.arr);
        }

        bigBinaryDecimal operator=(const uint32_t i) {
            arr.clear();
            arr.push_back(i);
            return *this;
        }

        bigBinaryDecimal operator=(bigBinaryDecimal &&o) {
            arr = move(o.arr);
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return *this;
        }

        bigBinaryDecimal operator=(bigBinaryDecimal &o) {
            arr = o.arr;
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return *this;
        }

        bigBinaryDecimal operator+=(bigBinaryDecimal &o) {
            if (o.arr.empty())
                return *this;
            while (o.sizeInByte() > this->sizeInByte())
                this->arr.push_front(0);
            auto iter = this->arr.begin();
            for (uint32_t i = 0; i < this->arr.size() - o.arr.size(); i++) {
                iter++;
            }
            auto oiter = o.arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0;
            for (; iter != this->arr.end(); ++iter, ++oiter) {
                uni.uint64 = (uint64_t) (*iter) + uni.uint32[1] + *oiter;
                *iter = uni.uint32[0];
            }
            assert(0 == uni.uint32[1]);
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return *this;
        }

        bigBinaryDecimal operator+=(bigBinaryDecimal &&o) {
            if (o.arr.empty())
                return *this;
            while (o.sizeInByte() > this->sizeInByte())
                this->arr.push_front(0);
            auto iter = this->arr.begin();
            for (uint32_t i = 0; i < this->arr.size() - o.arr.size(); i++) {
                iter++;
            }
            auto oiter = o.arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0;
            for (; iter != this->arr.end(); ++iter, ++oiter) {
                uni.uint64 = (uint64_t) (*iter) + uni.uint32[1] + *oiter;
                *iter = uni.uint32[0];
            }
            assert(0 == uni.uint32[1]);
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return *this;
        }

        bigBinaryDecimal operator+(bigBinaryDecimal &o) {
            bigBinaryDecimal newBigBinaryDecimal = *this;
            if (o.arr.empty())
                return newBigBinaryDecimal;
            while (newBigBinaryDecimal.arr.size() < o.arr.size())
                newBigBinaryDecimal.arr.push_front((uint32_t) 0);
            auto iter = newBigBinaryDecimal.arr.begin();
            for (uint32_t i = 0; i < newBigBinaryDecimal.arr.size() - o.arr.size(); i++) {
                iter++;
            }
            auto oiter = o.arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0;
            for (; iter != newBigBinaryDecimal.arr.end(); ++iter, ++oiter) {
                uni.uint64 = (uint64_t) (*iter) + uni.uint32[1] + *oiter;
                *iter = uni.uint32[0];
            }
            assert(0 == uni.uint32[1]);
            return newBigBinaryDecimal;
        }

        bigBinaryDecimal operator+(bigBinaryDecimal &&o) {
            bigBinaryDecimal newBigBinaryDecimal = *this;
            if (o.arr.empty())
                return newBigBinaryDecimal;
            while (newBigBinaryDecimal.arr.size() < o.arr.size())
                newBigBinaryDecimal.arr.push_front((uint32_t) 0);
            auto iter = newBigBinaryDecimal.arr.begin();
            for (uint32_t i = 0; i < newBigBinaryDecimal.arr.size() - o.arr.size(); i++) {
                iter++;
            }
            auto oiter = o.arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0;
            for (; iter != newBigBinaryDecimal.arr.end(); ++iter, ++oiter) {
                uni.uint64 = (uint64_t) (*iter) + uni.uint32[1] + *oiter;
                *iter = uni.uint32[0];
            }
            assert(0 == uni.uint32[1]);
            return newBigBinaryDecimal;
        }

        bigBinaryDecimal operator*=(const uint32_t i) {
            uint32_t carryBit = 0, temp = 0;
            if (this->arr.empty()) {
                this->arr.push_back(i);
                return *this;
            }
            auto iter = this->arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0;
            for (; iter != this->arr.end(); ++iter) {
                uni.uint64 = (uint64_t) (*iter) * i + uni.uint32[1];
                *iter = uni.uint32[0];
            }
            this->arr.push_back(uni.uint32[1]);
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return *this;
        }

        bigBinaryDecimal operator*(const uint32_t i) {
            bigBinaryDecimal newBigBinaryDecimal = *this;
            uint32_t carryBit = 0, temp = 0;
            if (newBigBinaryDecimal.arr.empty()) {
                newBigBinaryDecimal.arr.push_back(i);
                return newBigBinaryDecimal;
            }
            auto iter = newBigBinaryDecimal.arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0;
            for (; iter != newBigBinaryDecimal.arr.end(); ++iter) {
                uni.uint64 = (uint64_t) (*iter) * i + uni.uint32[1];
                *iter = uni.uint32[0];
            }
            newBigBinaryDecimal.arr.push_back(uni.uint32[1]);
            return newBigBinaryDecimal;
        }

        bigBinaryDecimal operator-=(bigBinaryDecimal &o) {
            if (o.arr.empty())
                return *this;
            while (o.sizeInByte() > this->sizeInByte())
                this->arr.push_front(0);
            auto iter = this->arr.begin();
            for (uint32_t i = 0; i < this->arr.size() - o.arr.size(); i++) {
                iter++;
            }
            auto oiter = o.arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0x0000000100000000;
            for (; iter != this->arr.end(); ++iter, ++oiter) {
                uni.uint64 = (uint64_t) (*iter) - *oiter - ((uni.uint32[1] == 0) ? 1 : 0);
                *iter = uni.uint32[0];
                uni.uint64 = 0x0000000100000000;
            }
            assert(1 == uni.uint32[1]);
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return *this;
        }

        bigBinaryDecimal operator-=(bigBinaryDecimal &&o) {
            if (o.arr.empty())
                return *this;
            while (o.sizeInByte() > this->sizeInByte())
                this->arr.push_front(0);
            auto iter = this->arr.begin();
            for (uint32_t i = 0; i < this->arr.size() - o.arr.size(); i++) {
                iter++;
            }
            auto oiter = o.arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0x0000000100000000;
            for (; iter != this->arr.end(); ++iter, ++oiter) {
                uni.uint64 = (uint64_t) (*iter) - *oiter - ((uni.uint32[1] == 0) ? 1 : 0);
                *iter = uni.uint32[0];
                uni.uint64 = 0x0000000100000000;
            }
            assert(1 == uni.uint32[1]);
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return *this;
        }

        bigBinaryDecimal operator-(bigBinaryDecimal &o) {
            bigBinaryDecimal newBigBinaryDecimal = *this;
            if (o.arr.empty())
                return newBigBinaryDecimal;
            while (newBigBinaryDecimal.arr.size() < o.arr.size())
                newBigBinaryDecimal.arr.push_front((uint32_t) 0);
            auto iter = newBigBinaryDecimal.arr.begin();
            for (uint32_t i = 0; i < newBigBinaryDecimal.arr.size() - o.arr.size(); i++) {
                iter++;
            }
            auto oiter = o.arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0x0000000100000000;
            for (; iter != newBigBinaryDecimal.arr.end(); ++iter, ++oiter) {
                uni.uint64 = (uint64_t) (*iter) - *oiter - ((uni.uint32[1] == 0) ? 1 : 0);
                *iter = uni.uint32[0];
                uni.uint64 = 0x0000000100000000;
            }
            assert(1 == uni.uint32[1]);
            return newBigBinaryDecimal;
        }

        bigBinaryDecimal operator-(bigBinaryDecimal &&o) {
            bigBinaryDecimal newBigBinaryDecimal = *this;
            if (o.arr.empty())
                return newBigBinaryDecimal;
            while (newBigBinaryDecimal.arr.size() < o.arr.size())
                newBigBinaryDecimal.arr.push_front((uint32_t) 0);
            auto iter = newBigBinaryDecimal.arr.begin();
            for (uint32_t i = 0; i < newBigBinaryDecimal.arr.size() - o.arr.size(); i++) {
                iter++;
            }
            auto oiter = o.arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0x0000000100000000;
            for (; iter != newBigBinaryDecimal.arr.end(); ++iter, ++oiter) {
                uni.uint64 = (uint64_t) (*iter) - *oiter - ((uni.uint32[1] == 0) ? 1 : 0);
                *iter = uni.uint32[0];
                uni.uint64 = 0x0000000100000000;
            }
            assert(1 == uni.uint32[1]);
            return newBigBinaryDecimal;
        }

        bigBinaryDecimal operator/=(const uint32_t i) {
            assert(i > 0);
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0;
            auto iter = this->arr.end();
            do {
                --iter;
                uni.uint32[0] = *iter;
                uint32_t remainder = (uint32_t) (uni.uint64 % i);
                uint32_t quotient = (uint32_t) (uni.uint64 / i);
                *iter = quotient;
                uni.uint32[1] = remainder;
            } while (iter != this->arr.begin());
            uni.uint32[0] = 0;
            this->arr.push_front((uint32_t) (uni.uint64 / i));
            assert(0 == this->arr.back());
            this->arr.pop_back();
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return *this;
        }

        bigBinaryDecimal operator/(const uint32_t i) {
            bigBinaryDecimal newBigBinaryDecimal = *this;
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0;
            auto iter = newBigBinaryDecimal.arr.end();
            do {
                --iter;
                uni.uint32[0] = *iter;
                uint32_t remainder = (uint32_t) (uni.uint64 % i);
                uint32_t quotient = (uint32_t) (uni.uint64 / i);
                uni.uint32[1] = remainder;
            } while (iter != newBigBinaryDecimal.arr.begin());
            uni.uint32[0] = 0;
            newBigBinaryDecimal.arr.push_front((uint32_t) (uni.uint64 / i));
            assert(0 == newBigBinaryDecimal.arr.back());
            newBigBinaryDecimal.arr.pop_back();
            return *this;
        }

        bool operator>=(const uint32_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return this->arr.back() >= i;
        }

        bool operator>(const uint32_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return (this->arr.back() > i) || (this->arr.back() == i && this->arr.size() > 1);
        }

        bool operator<=(const uint32_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return (this->arr.back() < i) || (this->arr.back() == i && this->arr.size() == 1);
        }

        bool operator<(const uint32_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return this->arr.back() < i;
        }

        bool operator==(const uint32_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return this->arr.back() == i && this->arr.size() == 1;
        }

        bool operator>=(const uint64_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return (uint64_t) this->arr.back() >= i;
        }

        bool operator>(const uint64_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return ((uint64_t) this->arr.back() > i) || ((uint64_t) this->arr.back() == i && this->arr.size() > 1);
        }

        bool operator<=(const uint64_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return ((uint64_t) this->arr.back() < i) || ((uint64_t) this->arr.back() == i && this->arr.size() == 1);
        }

        bool operator<(const uint64_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return (uint64_t) this->arr.back() < i;
        }

        bool operator==(const uint64_t i) {
            assert(!this->arr.empty());
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
            return (uint64_t) this->arr.back() == i && this->arr.size() == 1;
        }

        size_t sizeInByte() {
            return this->arr.size() * sizeof(uint32_t);
        }

        void clear() {
            this->arr.clear();
        }

        uint32_t *getData() {
            if (this->arr.size() <= 0)
                return nullptr;
            uint32_t *ret = new uint32_t[this->arr.size()];
            uint32_t i = 0;
            for (auto iter = this->arr.begin(); iter != this->arr.end(); ++iter) {
                ret[i++] = *iter;
            }
            return ret;
        }

        void cutShort(bigBinaryDecimal &o) {
            auto oiter = o.arr.end();
            uint32_t i = 0;
            do {
                --oiter;
                i++;
                if (0 != *oiter) {
                    if (*oiter > 1 || (oiter != o.arr.begin() && --oiter != o.arr.begin()))
                        break;
                }
            } while (oiter != o.arr.begin());
            while (this->arr.size() > i) {
                this->arr.pop_front();
            }
            oiter = this->arr.begin();
            union {
                uint64_t uint64;
                uint32_t uint32[2];
            } uni;
            uni.uint64 = 0;
            while (oiter != this->arr.end()) {
                uni.uint64 = (uint64_t) (*oiter) + uni.uint32[1] + ((oiter == this->arr.begin()) ? 1 : 0);
                *oiter = uni.uint32[0];
                ++oiter;
            }
            assert(0 == uni.uint32[1]);
            while (this->arr.size() > 1 && 0 == this->arr.front())
                this->arr.pop_front();
        }

    };

    bigBinaryDecimal C;
    bigBinaryDecimal A;

public:
    arithmeticCoder(const string str) {
        assert(str.length() > 0);
        uint32_t counter[256] = {0};
        this->str = str;
        this->Encoded = false;
        this->Decoded = true;
        this->code = nullptr;
        for (uint32_t i = 0; i < str.length(); i++) {
            counter[str[i]]++;
        }
        {
            uint32_t temp = 0, j = 0;
            for (uint32_t i = 0; i < 256; i++) {
                if (counter[i] > 0) {
                    temp++;
                    j = i;
                }
            }
            if (1 == temp) {
                code = new uint32_t[258];
                code[0] = 258;
                code[1] = str.length();
                memset(code + 2, 0, 256 * sizeof(uint32_t));
                code[2 + j] = 1;
                Encoded = true;
                return;
            };
        }
        for (uint32_t i = 0; i < 256; i++) {
            uint64_t posibility = 0x0000000100000000;
            p[i] = (uint32_t) (posibility * counter[i] / str.length());
        }
        {
            uint32_t temp = 0;
            uint64_t loss = 0x0000000100000000;
            uint32_t i;
            for (i = 0x80000000; i > 0; i >>= 1) {
                temp |= i;
                bool b = true;
                for (uint32_t j = 0; j < 256; j++) {
                    b = b && ((p[j] & temp) > 0 || p[j] == 0);
                }
                if (b)
                    break;
            }
            for (uint32_t i = 0; i < 256; i++) {
                p[i] &= temp;
            }
            for (uint32_t i = 0; i < 256; i++) {
                loss -= p[i];
            }
            loss /= i;
            pair<uint32_t, uint32_t> sortPair[256];
            for (uint32_t i = 0; i < 256; i++) {
                sortPair[i] = make_pair(p[i], i);
            }
            sort(sortPair, sortPair + 256, greater<pair<uint32_t, uint32_t >>());
            for (uint32_t j = 0; j < loss; j++) {
                p[sortPair[j].second] += i;
            }
        }
        P[0] = 0;
        for (uint32_t i = 1; i < 256; i++)
            P[i] = P[i - 1] + p[i - 1];
        C = 0;
    }

    arithmeticCoder(uint32_t *str) {
        uint32_t lengthOfArray = str[0];
        this->Decoded = false;
        this->Encoded = true;
        this->code = new uint32_t[str[0]];
        memcpy(this->code, str, str[0] * sizeof(uint32_t));
        memcpy(&p[0], str + 2, 256 * sizeof(uint32_t));
        P[0] = 0;
        for (uint32_t i = 1; i < 256; i++) {
            P[i] = P[i - 1] + p[i - 1];
        }
        C = bigBinaryDecimal(str + 258, lengthOfArray - 258);
    }

    ~arithmeticCoder() {
        if (nullptr != code) {
            delete[]code;
            this->code = nullptr;
        }
    }

    void encode() {
        assert(Decoded);
        if (this->Encoded)
            return;
        size_t length = str.length();
        this->Encoded = true;
        for (uint32_t i = 0; i < str.length(); i++) {
            C += A * P[str[i]];
            A *= p[str[i]];
            if (i % 1000 == 0) {
                cout << endl << "complete: " << (long double) i * 100 / length << "%" << endl;
                cout << A.sizeInByte() << endl;
                cout << C.sizeInByte() << endl << endl;
            }
        }

        cout << "finish" << endl;
        cout << A.sizeInByte() << endl;
        cout << C.sizeInByte() << endl << endl;

        C.cutShort(A);

        this->code = new uint32_t[258 + C.sizeInByte() / sizeof(uint32_t)];
        code[0] = 258 + C.sizeInByte() / sizeof(uint32_t);
        code[1] = length;
        for (uint32_t i = 0; i < 256; i++) {
            code[i + 2] = p[i];
        }
        uint32_t *data = C.getData();
        size_t len = C.sizeInByte();
        memcpy(code + 258, data, len);
        delete[] data;
    }

    void decode() {
        assert(Encoded);
        if (Decoded)
            return;
        char *hhh = new char[code[1] + 1];
        hhh[code[1]] = 0;
        if (258 == this->code[0]) {
            for (uint32_t i = 0; i < 256; i++) {
                if (0 != code[2 + i]) {
                    for (uint32_t j = 0; j < code[1]; j++) {
                        hhh[j] = (char) i;
                    }
                    str = hhh;
                    return;
                }
            }
            cout << "invalid data!" << endl;
        }
        for (uint32_t ii = 0; ii < code[1]; ii++) {
            uint32_t i;
            for (i = 0; i < 255; i++) {
                if (C < ((uint64_t) p[i] + P[i]))
                    break;
            }
            hhh[ii] = i;
            C -= P[i];
            C /= p[i];
        }
        str = hhh;
        delete[] hhh;
        hhh = nullptr;
    }

    uint32_t *getData() {
        return code;
    }

    string getString() {
        return str;
    }
};

#endif //ARITHMETICCODER_H
