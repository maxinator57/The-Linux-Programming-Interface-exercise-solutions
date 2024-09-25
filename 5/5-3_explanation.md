### Exercise 5-3 explanation

If the file `f1.txt` doesn't exist yet and we run 
```console
./atomic_append f1.txt 1000000 & ./atomic_append f1.txt 1000000
```
we will get a file named `f1.txt` the size of which will be exactly $1000000 + 1000000 = 2000000$ bytes.

However, if the file `f2.txt` doesn't exist either and we run
```console
./atomic_append f2.txt 1000000 x & ./atomic_append f2.txt 1000000 x
```
we will get a file named `f2.txt` the size of which will most likely be less than $2000000$ bytes (but might be exactly $2000000$, which is very unlikely, though).

This happens due to the fact that, according to the `open(2)` man page (see the paragraph **"Open file descriptions"** in the **"NOTES"** section),

"Each `open()` of a file creates a new open file description; thus, there may be multiple open file descriptions corresponding to a file inode."

This means that in both runs each of the two `./atomic_append` processes gets a file descriptor referring to its own open file description in the system-wide open file table. Each open file description maintains its own file offset. Hence, in the second run (when we call `lseek(fd, 0, SEEK_END)` before writing each byte to `f2`), the actual size of `f2` might be increased between this call and the call `write(fd, buf, 1)` by a write performed by the other `./atomic_append` process. Therefore, when calling `write(fd, buf, 1)` the file offset of the given `./atomic_append` process might be stricly less than the actual size of `f2`, which causes the `write` call to overwrite an already written byte instead of appending to `f2`.

Yet, this problem doesn't occur in the first run, since, although each of the two `./atomic_append` processes has its own `f1` file offset, whe update of the offset so that it points to the actual end of the file and the write of $1$ byte to `f1` are performed as a single action, i.e. atomically, thanks to the `O_APPEND` flag specified when opening `f1`. Thus, each write to `f1` in the first run is an actual append and the resulting size of `f1` will indeed be $2000000$.
