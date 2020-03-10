#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

int main()
{
    int Q, i = 0;
    cin >> Q;
    map<string, vector<string>> info;
    vector<string> order;
    while (i < Q)
    {
        string command;
        cin >> command;
        if (command == "NEW_BUS")
        {
            string bus, temp;
            int stop_count;
            cin >> bus;
            order.push_back(bus);
            cin >> stop_count;
            int j = 0;
            while (j++ < stop_count)
            {
                cin >> temp;
                info[bus].push_back(temp);
            }
            continue;
        }
        if (command == "BUSES_FOR_STOP")
        {
            string stop;
            cin >> stop;
            int counter = 0;
            for (auto c : order){
                for (auto k : info[c]) {
                    if (k == c) {
                        cout << c << " ";
                        counter++;
                        break;
                    }
                }
            }
            if (counter == 0)
                cout << "No stop" << endl;
            else
                cout << endl;
            continue;
        }
        if (command == "STOPS_FOR_BUS")
        {
            string bus;
            cin >> bus;
            if (info[bus].size() == 0)
            {
                cout << "No bus" << endl;
            } else
            {
                for (auto c : info[bus])
                {
                    int counter = 0;
                    cout << "Stop " << c <<": ";
                    for (auto j : order)
                    {
                        int indicator = 0;
                        for (auto k : info[j])
                        {
                            if (k == c)
                            {
                                indicator = 1;
                                break;
                            }
                        }
                        if (indicator && j != bus)
                        {
                            cout << j << " ";
                            counter++;
                        }
                    }
                    if (counter == 0)
                        cout << "no interchange";
                    cout << endl;
                }
            }
            continue;
        }
        if (command == "ALL_BUSES")
        {
            if (info.size() != 0)
            {
                for (auto c : info)
                {
                    cout << "Bus " << c.first <<": ";
                    vector<string> temp = c.second;
                    for (auto j : temp)
                        cout << j << " ";
                    cout << endl;
                }
            } else
                cout << "No buses" << endl;
        }
    }
}