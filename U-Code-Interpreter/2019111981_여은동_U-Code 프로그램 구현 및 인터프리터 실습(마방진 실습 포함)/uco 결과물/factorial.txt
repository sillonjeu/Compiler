
 line       object           ucode  source program

   1    (35    2    2)     main       proc           2     2     2
   2    (38    2    1)                sym            2     1     1
   3    (38    2    2)                sym            2     2     1
   4    (23          )                ldp       
   5    (21    2    1)                lda            2     1
   6    (29   -1     )                call       read
   7    (23          )                ldp       
   8    (19    2    1)                lod            2     1
   9    (29   -2     )                call       write
  10    (23          )                ldp       
  11    (19    2    1)                lod            2     1
  12    (29   18     )                call       factorial
  13    (24    2    2)                str            2     2
  14    (23          )                ldp       
  15    (19    2    2)                lod            2     2
  16    (29   -2     )                call       write
  17    (30          )                ret       
  18    (35    1    2)     factorial  proc           1     2     2
  19    (38    2    1)                sym            2     1     0
  20    (19    2    1)                lod            2     1
  21    (20    1     )                ldc            1
  22    (17          )                eq        
  23    (28   27     )                fjp        $$0
  24    (20    1     )                ldc            1
  25    (31          )                retv      
  26    (26   36     )                ujp        $$1
  27    (34          )     $$0        nop       
  28    (19    2    1)                lod            2     1
  29    (23          )                ldp       
  30    (19    2    1)                lod            2     1
  31    (20    1     )                ldc            1
  32    ( 7          )                sub       
  33    (29   18     )                call       factorial
  34    ( 8          )                mult      
  35    (31          )                retv      
  36    (34          )     $$1        nop       
  37    (30          )                ret       
  38    (37    0     )                bgn            0
  39    (23          )                ldp       
  40    (29    1     )                call       main
  41    (36          )                end       


   ****    Result    ****

 5 120


             ##### Statistics #####


    ****  Static Instruction Counts  ****

sub    =  1       mult   =  1       eq     =  1       lod    =  6       
ldc    =  3       lda    =  1       ldp    =  6       str    =  1       
ujp    =  1       fjp    =  1       call   =  6       ret    =  2       
retv   =  2       nop    =  2       proc   =  2       end    =  1       
bgn    =  1       sym    =  3       

    ****  Dynamic instruction counts  ****

sub    =  4         mult   =  4         eq     =  5         lod    =  16        
ldc    =  10        lda    =  1         ldp    =  9         str    =  1         
fjp    =  5         call   =  9         ret    =  1         retv   =  5         
nop    =  4         proc   =  6         end    =  1         bgn    =  1         
sym    =  7         

 Executable instruction count  =   76

 Total execution cycle         =   1250
