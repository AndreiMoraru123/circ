template <typename InputIt1, typename InputIt2, typename OutputIt>
OutputIt zip(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
             OutputIt dest) {
  auto it1 = first1;
  auto it2 = first2;

  while (it1 != last1) {
    *dest++ = *it1++;
  };

  while (it2 != last2) {
    *dest++ = *it2++;
  };
  return dest;
}