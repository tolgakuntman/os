make all
port=5678
clients=5
echo -e "starting gateway "
valgrind --leak-check=full --show-leak-kinds=all ./sensor_gateway $port $clients &
sleep 3
echo -e 'starting 5 sensor nodes'
./sensor_node 15 1 127.0.0.1 $port &

./sensor_node 21 2 127.0.0.1 $port &

./sensor_node 37 3 127.0.0.1 $port &

./sensor_node 132 4 127.0.0.1 $port &

./sensor_node 142 11 127.0.0.1 $port &
sleep 10
killall sensor_node
sleep 1
killall sensor_gateway
