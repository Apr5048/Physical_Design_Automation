case=p2-4
best_seed=1
best=999999999999999999
for i in $(seq 1 20)
do
    echo "seed" $i
    cur=$(./../bin/hw2 ../testcases/$case.nets ../testcases/$case.cells ../output/$case.out $i)
    if [ $best -gt $cur ]; then
    	best=$cur
      best_seed=$i
    fi
    echo "cur=$cur best=$best best_seed=$best_seed"
done
