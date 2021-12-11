#include <iostream>
#include <string>
#include <list>
#include <map>
#include <iterator>
#include <queue>
#include <climits>
using namespace std;

class Node
{
    int cost;
    string name;
    Node *next;

public:
    Node(int f, string n)
    {
        cost = f;
        name = n;
        next = NULL;
    }
    friend class Graph;
};
class Graph
{
    int n; //total routers
    list<Node> *NodesList;
    map<string, int> nameToIndexMap;
    

public:
    vector<vector<string>> pathSeq;
    int min_path = INT_MAX;
    Graph(int num)
    {
        n = num;
        NodesList = new list<Node>[n];
    }

    void create()
    {
        int choice = 1;
        int dist = 0, j = 0;
        string name, source, destination;
        cout << "Enter names of " << n << " router(s): \n";
        for (int i = 0; i < n; i++)
        {
            cin >> name; //check before inserting if already present
            nameToIndexMap.insert(pair<string, int>(name, i));
        }
        while (choice)
        {
            cout << "Enter source, destination and cost of edge \n";
            cin >> source >> destination >> dist;
            if (nameToIndexMap.find(source) == nameToIndexMap.end() || nameToIndexMap.find(destination) == nameToIndexMap.end())
            {
                cout << "Invalid source or destination name\n";
            }
            else
            {
                Node *endNode = new Node(dist, destination);
                Node *startNode = new Node(dist, source);

                map<string, int>::iterator itr;
                itr = nameToIndexMap.find(source);
                NodesList[itr->second].push_back(*endNode);
                itr = nameToIndexMap.find(destination);
                NodesList[itr->second].push_back(*startNode);
            }
            cout << "Enter 1 to continue \n";
            cin >> choice;
        }
    }
    void display()
    {
        map<string, int>::iterator itr;

        for (itr = nameToIndexMap.begin(); itr != nameToIndexMap.end(); ++itr)
        {
            cout << "Routers connected to " << itr->first << " are : \n";
            list<Node>::iterator it;

            for (it = NodesList[itr->second].begin(); it != NodesList[itr->second].end(); ++it)
            {
                cout << "Name: " << it->name << "\t Cost: " << it->cost << endl;
            }
        }
    }
    struct CompareCost
    {
        bool operator()(Node const &n1, Node const &n2)
        {
            return n1.cost > n2.cost;
        }
    };
    void dijkstra(string source)
    {
        priority_queue<Node, vector<Node>, CompareCost> pq;
        list<string> path;
        vector<int> dist(n, INT_MAX);
        pq.push(Node(0, source));
        if (nameToIndexMap.find(source) == nameToIndexMap.end())
        {
            cout << "Invalid source name\n";
        }
        else
        {
            map<string, int>::iterator itr, xyz;
            itr = nameToIndexMap.find(source);
            dist[itr->second] = 0;
            while (!pq.empty())
            {
                string u = pq.top().name;
                pq.pop();
                itr = nameToIndexMap.find(u);
                list<Node>::iterator i;
                for (i = NodesList[itr->second].begin(); i != NodesList[itr->second].end(); ++i)
                {
                    string v = (*i).name;
                    int weight = (*i).cost;
                    xyz = nameToIndexMap.find(v);
                    if (dist[xyz->second] > dist[itr->second] + weight)
                    {
                        dist[xyz->second] = dist[itr->second] + weight;
                        pq.push(Node(dist[xyz->second], v));
                    }
                }
            }
            printf("Vertex   Distance from Source\n");
            for(auto i: nameToIndexMap)
            {
                cout<<i.first<<"\t\t"<<dist[i.second]<<"\n";
            }
        }
    }
    void printAllPaths(string s, string d)
    {
        if (nameToIndexMap.find(s) == nameToIndexMap.end() || nameToIndexMap.find(d) == nameToIndexMap.end())
        {
            cout << "Invalid source or destination name\n";
            return;
        }
        bool *visited = new bool[n];
        string *path = new string[n];
        int path_index = 0; // Initialize path[] as empty
        int path_cost = 0;
        for (int i = 0; i < n; i++)
            visited[i] = false;
        cout << "Following are all possible paths from router " << s << " to " << d << ": \n";
        printAllPathsUtil(s, d, visited, path, path_index, path_cost);
    }
    void printAllPathsUtil(string u, string d, bool visited[],
                           string path[], int &path_index, int path_cost)
    {
        // Mark the current node and store it in path[]
        map<string, int>::iterator mapItrSrc, mapItrDest;
        mapItrSrc = nameToIndexMap.find(u);
        visited[mapItrSrc->second] = true;
        path[path_index] = u;
        path_index++;

        if (u == d)
        {
            if (path_cost <= min_path)
            {
                if (path_cost < min_path)
                {
                    pathSeq.clear();
                    min_path = path_cost;
                }
                vector<string> path_seq;
                for (int i = 0; i < path_index; i++)
                {
                    path_seq.push_back(path[i]);
                }
                pathSeq.push_back(path_seq);
            }

            for (int i = 0; i < path_index; i++)
            {
                cout << path[i] << " ";
            }
            cout << "Cost: " << path_cost << endl;
        }
        else // If current vertex is not destination
        {
            // Recur for all the vertices adjacent to current vertex
            list<Node>::iterator i;
            for (i = NodesList[mapItrSrc->second].begin(); i != NodesList[mapItrSrc->second].end(); ++i)
            {
                string num = i->name;
                mapItrDest = nameToIndexMap.find(num);
                int c = i->cost;
                path_cost += c;
                if (!visited[mapItrDest->second])
                    printAllPathsUtil(num, d, visited, path, path_index, path_cost);
                path_cost -= c;
            }
        }

        // Remove current vertex from path[] and mark it as unvisited
        path_index--;
        visited[mapItrSrc->second] = false;
    }
    void printEqualCostPaths(int packet_num)
    {
        cout << "Least cost from source to destination is " << min_path << endl;
        cout << "Following path(s) have least cost: \n";
        for (int i = 0; i < pathSeq.size(); i++)
        {
            for (int j = 0; j < pathSeq[i].size(); j++)
            {
                cout << pathSeq[i][j] << " ";
            }
            cout << endl;
        }
        if (pathSeq.size() > 1)
        {
            //use ecmp
            cout << "As there are multiple least cost paths we are using Round Robin mehtod of ECMP for load balancing.\n"
                 << packet_num << " packets will take following paths respectively: \n";
            int pathIndex = 0;
            for (int i = 0; i < packet_num; i++)
            {
                pathIndex = (pathIndex + 1) % pathSeq.size();
                for (int j = 0; j < pathSeq[pathIndex].size(); j++)
                {
                    cout << pathSeq[pathIndex][j] << " ";
                }
                cout << endl;
            }
        }
        else
        {
            cout << "As there's only one least cost path from source to destination all " << packet_num << " packets will take the above path.\n";
        }
    }
};

int main()
{
    cout << "I am considering it's an undirected graph i.e. \nAn edge between routers A and B implies the packet can go from either A to B or from B to A \n";
    cout << "Enter the number of routers.";
    int n, choice, c;
    string src, dest;
    cin >> n;
    Graph obj1(n);
    do
    {
        cout << "1.Make graph \n";
        cout << "2.Display \n";
        cout << "3.Print minimum cost of reaching each router from source.\n";
        cout << "4.Send packets from source to destination. (Use ECMP if there's more than one least cost path)\n";
        cin >> choice;
        switch (choice)
        {
        case 1:
            obj1.create();
            break;

        case 2:
            obj1.display();
            break;

        case 3:
            cout << "Enter name of source router: \n";
            cin >> src;
            obj1.dijkstra(src);
            break;

        case 4:
            obj1.min_path = INT_MAX;
            obj1.pathSeq.clear();
            cout << "Enter name of source and destiantion routers: \n";
            cin >> src >> dest;
            cout << "Enter number of packets: \n";
            cin >> n;
            obj1.printAllPaths(src, dest);
            obj1.printEqualCostPaths(n);
            break;
        }
        cout << "Press 1 to continue else press 0\n";
        cin >> c;
    } while (c == 1);
}