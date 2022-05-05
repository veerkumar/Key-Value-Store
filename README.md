<h1>Key-Value-Store</h1>

<b>Functional Spec: Design a scalable and efficient key-value store with immutable data. Data will be loaded at the initialization and it won't be modified during the service. User will give a key and the server should return the value.</b>

Designing scalable KV store involves answering at least two primary questions:
1) How to store the data: Before we answer this, we first need to answer what kind of queries the data storage layer should expect from the client?<br></br>
    a) Is there regular expression or range queries? or is it just a key ?
    
      This will help in deciding the data structure, do we want to use LSM, b+ trees or just hash maps. Random reads are more costly than the sequential ones hence we need to carefully choose data structure.<br></br>
    b) Will there be lots of deletes ?
    
      If deletes are permitted then we have to consider how to handle memory fragmentation or compression?

    c) what would be the size of the value ? is it just int or is it in M/G/TB's ?
    
      This will answer should we keep key-value in-memory or store values in HDD/SDD and just store the mapping in-memory.
      
Since our problem statement just says "read" queries, we will use hashmaps and to make our map light weight, instead of storing values in the map, we will store pointers to the corresponding values.

2) How to handle the connections?

    a) Do we have a million clients? or each client will send millions of requests? or both?
    
      This will help in determining how we set up our socket resources, if clients are in millions then probably they will not send requests so frequently, Hence we don't have to spin up a dedicated thread and keep waiting for the request. Instead we can use epoll to handle those clients and we will get notified when the request arrives. On the other hand, if we have a limited number of clients and very high request rate, we can have active threads which will keep churning the requests as soon as they land but again that will depend on the following question too.
      
    b) Requests are cpu intensive or I/O intensive?
    
     If a job involves lots of I/O ops, we don't really have to hold on to the cpu resources. We can again use an event driven approach to get notification when anything arrives on the I/O resource endpoint.

In our cases, requests are I/O bound and we are not limited by number of clients or requests, hence we used Boost::asio to handle the sockets (which internally uses epoll).
 


<h2>Performance</h2>

*Machine configuration:*
``` 
    CPU: 3 vCPU - Intel(R) Core(TM) i7-1068NG7 CPU @ 2.30GHz
    RAM: 8G
```

|No of key-value|   Avg Latency(Microsecond) | Mem after initialization| Mem during test  |
| ------------- |:--------------------------:| -----------------------:|-----------------:|
| 10k           |   100 to 200, median 130   | 16MB (0.2%)	       | 16MB (0.2%)      |
| 1 Million     |   100 to 200, median 130   | 224MB(2.8%)             | 224MB(2.8%)      |


<h2>Execution</h2>

1) <h4>Generate data</h4> Based on data size requirement, edit data_generator.py and modify the value of "data_size".

```
python3 data_generator.py
```

3) <h4>Build</h4>

```
make
```

3) Run the server with above generated data file.

```
./Server testdata_1M.data
```

4) Now run the client. Client will also take same data file as input as it will randomly pick a key and query the server. In kv_store_client.cpp, we have two paramter to control concurrency (NUM_CLIENT) and total number of requests (NUM_REQUEST). Currently set to 10 clients and 1 Million requests.

```
./Client testdata_1M.data
``` 
