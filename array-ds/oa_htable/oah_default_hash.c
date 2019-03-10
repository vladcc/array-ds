#define OAH_DEFAULT_HASH jenkins_one_at_a_time_hash

// Thanks, Bob!
static unsigned int jenkins_one_at_a_time_hash(
    const unsigned char * key, int length
    )
{
  unsigned int i = 0;
  unsigned int hash = 0;

  while (i != length)
  {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }

  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;

  return hash;
}
