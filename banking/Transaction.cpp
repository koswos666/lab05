bool Transaction::Make(Account& from, Account& to, int sum) {
  if (from.id() == to.id()) throw std::logic_error("invalid action");
  if (sum < 0) throw std::invalid_argument("sum can't be negative");
  if (sum < 100) throw std::logic_error("too small");

  Guard guard_from(from);
  Guard guard_to(to);
  if (from.GetBalance() < sum + fee_) {
    return false;
  }
  try {
    SaveToDataBase(from, to, sum);
  } catch (...) {
    return false;
  }
  Credit(to, sum);
  Debit(from, sum + fee_);
  return true;
}
