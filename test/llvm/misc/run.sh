for i in $(seq 100); do
  lli $2 sum.bc
  let i=i+1
done
