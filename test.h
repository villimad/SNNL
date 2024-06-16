#ifndef TEST_H
#define TEST_H


class Test
{
public:
    void run();
    void diff();
    void diffForwardSimpleTest();
    void diffForward();
    void diffForwardRecTest();
    void diffVecSum();
    void reluSimpleTest();
    void diffReluSimpleTest();
    void translatorSimpleTest();
    void solveVecSumHardTest();
    void mulSimpleTest();
    void mulSimpleTestZero();
    void plusMulSimpleTest();
    void plusMulSimpleTestZero();
    void perseptronTest();
    void perseptron2Test();
    void block_treeTest();
};

#endif // TEST_H
