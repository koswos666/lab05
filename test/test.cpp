class TransactionTestFriend : public Transaction {
public:
    void TestCredit(Account& acc, int sum) { Credit(acc, sum); }
    bool TestDebit(Account& acc, int sum) { return Debit(acc, sum); }
};

TEST(TransactionTest, EdgeCases) {
    TransactionTestFriend transaction;
    Account acc1(1, 99);
    Account acc2(2, 0);
    
    // Проверка sum < 100
    ASSERT_THROW(transaction.Make(acc1, acc2, 99), std::logic_error);
    
    // Проверка баланса
    Account acc3(3, 100);
    ASSERT_FALSE(transaction.Make(acc3, acc2, 100));
}
