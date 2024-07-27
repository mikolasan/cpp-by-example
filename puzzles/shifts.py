x = [13, 5, 6, 2, 5]
y = [5, 2, 5, 13]

# max_array_size = 99 - 2x 64-bit integers 
# min_value = -1000
# max_value = 1000

# 2001 values

# 2^11

len_x = len(x)
len_y = len(y)
size = max(x, y)
match_map = {}
for i in range(size):
  if i < len_x:
    if x[i] in match_map:
      match_map[x[i]] += 1
    else:
      match_map[x[i]] = 1
  if i < len_y:
    if y[i] in match_map:
      match_map[y[i]] += 1
    else:
      match_map[y[i]] = 1
  