#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

// Base Sensor Class

class Sensor {
public:
    virtual ~Sensor() = default;
    virtual double readData() const = 0;
    virtual string getType() const = 0;
};

// Derived sensor classes

class TemperatureSensor : public Sensor {
public:
    double readData() const override {
        return 60.0 + static_cast<double>(rand()) / RAND_MAX * 20; // Simulated temperature (60°C - 80°C)
    }
    string getType() const override {
        return "Temperature";
    }
};

class PressureSensor : public Sensor {
public:
    double readData() const override {
        return 200.0 + static_cast<double>(rand()) / RAND_MAX * 50; // Simulated pressure (200 - 250 psi)
    }
    string getType() const override {
        return "Pressure";
    }
};

class VibrationSensor : public Sensor {
public:
    double readData() const override {
        return 0.5 + static_cast<double>(rand()) / RAND_MAX * 1.0; // Simulated vibration (0.5 - 1.5 m/s^2)
    }
    string getType() const override {
        return "Vibration";
    }
};

// Factory for Creating Sensors
class SensorFactory {
public:
    static shared_ptr<Sensor> createSensor(const string& type) {
        if (type == "Temperature")
            return make_shared<TemperatureSensor>();
        else if (type == "Pressure")
            return make_shared<PressureSensor>();
        else if (type == "Vibration")
            return make_shared<VibrationSensor>();
        else
            throw invalid_argument("Unknown sensor type");
    }
};

// Decorator for Sensors (e.g., for Alerts or Logging)
class SensorDecorator : public Sensor {
protected:
    shared_ptr<Sensor> wrappedSensor;
public:
    SensorDecorator(shared_ptr<Sensor> sensor) : wrappedSensor(sensor) {}
};

class LoggingSensor : public SensorDecorator {
private:
    double alertThreshold;
public:
    LoggingSensor(shared_ptr<Sensor> sensor, double threshold) 
        : SensorDecorator(sensor), alertThreshold(threshold) {}

    double readData() const override {
        double data = wrappedSensor->readData();
        logData(data);
        if (data > alertThreshold) {
            cout << "[ALERT] " << wrappedSensor->getType() 
                 << " Sensor exceeds threshold! Reading: " << data 
                 << ", Threshold: " << alertThreshold << endl;
        }
        return data;
    }

    string getType() const override {
        return wrappedSensor->getType();
    }

    void logData(double data) const {
        ofstream logFile("machine_logs.txt", ios::app);
        logFile << wrappedSensor->getType() << " Sensor: " << data << endl;
        logFile.close();
    }
};

// MachineHealthMonitor Class (Composition)
class MachineHealthMonitor {
private:
    vector<shared_ptr<Sensor>> sensors;
public:
    void addSensor(const shared_ptr<Sensor>& sensor) {
        sensors.push_back(sensor);
    }

    void monitor() {
        cout << "Monitoring Machine Health:" << endl;
        for (const auto& sensor : sensors) {
            double data = sensor->readData();
            cout << sensor->getType() << " Sensor Reading: " << data << endl;
        }
    }

    void predictMaintenance() {
        cout << "\nPredictive Maintenance Analysis:" << endl;
        for (const auto& sensor : sensors) {
            string type = sensor->getType();
            double data = sensor->readData();

            if (type == "Temperature" && data > 75.0) {
                cout << "Prediction: High temperature indicates overheating. Schedule cooling system check." << endl;
            } else if (type == "Pressure" && data > 240.0) {
                cout << "Prediction: High pressure may cause hydraulic failure. Inspect hydraulic lines." << endl;
            } else if (type == "Vibration" && data > 1.2) {
                cout << "Prediction: Excessive vibration detected. Check bearings or engine mounts." << endl;
            } else {
                cout << "Prediction: " << type << " sensor readings are within normal range." << endl;
            }
        }
    }
};

// Main Function
int main() {
    srand(static_cast<unsigned>(time(0)));

    MachineHealthMonitor monitor;

    auto tempSensor = make_shared<LoggingSensor>(SensorFactory::createSensor("Temperature"), 75.0);
    auto pressureSensor = make_shared<LoggingSensor>(SensorFactory::createSensor("Pressure"), 240.0);
    auto vibrationSensor = make_shared<LoggingSensor>(SensorFactory::createSensor("Vibration"), 1.2);

    // Add sensors to the monitor
    monitor.addSensor(tempSensor);
    monitor.addSensor(pressureSensor);
    monitor.addSensor(vibrationSensor);

    int i = 0;
    // Create sensors with logging and dynamic thresholds
    while (1){
        i++;

        monitor.monitor();
        monitor.predictMaintenance();

        cout << "Logs saved to machine_logs.txt" << endl;
        cout << endl;
        cout << "*******************************************************************";
        if (i > 5){
            int choice;
            cout << endl << "If you want to continue monitoring press 1 else any key: ";
            cin >> choice;

            if (choice == 1){
                monitor.monitor();
                monitor.predictMaintenance();
            }
            else{
                break;
            }
        }
    }

    return 0;
}