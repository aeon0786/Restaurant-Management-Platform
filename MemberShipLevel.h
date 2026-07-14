#pragma once
#include <string>
using namespace std;
class Customer;
class MemberShipLevel
{
    public:
        virtual ~MemberShipLevel() = default;
        virtual string getLevelName() const = 0;
        virtual double getDiscountRate() const = 0;
        virtual double getPointsMultiplier() const = 0;
        virtual double getDeliveryCostMultiplier() const = 0;
        virtual int getNextThreshold() const = 0;
        virtual int getMonthlyCouponCount() const = 0;
        virtual void changeLevel(Customer *customer) = 0;
};

class NormalLevel : public MemberShipLevel
{
    string getLevelName() const override { return "Normal"; }
    double getDiscountRate() const override { return 0.0; }
    double getPointsMultiplier() const override { return 1.0; }
    double getDeliveryCostMultiplier() const override { return 1.0; }
    int getNextThreshold() const override { return 100; }
    int getMonthlyCouponCount() const override { return 0; }
    void changeLevel(Customer *) override;
};
class SilverLevel : public MemberShipLevel
{
    string getLevelName() const override { return "Silver"; }
    double getDiscountRate() const override { return 0.05; }
    double getPointsMultiplier() const override { return 1.2; }
    double getDeliveryCostMultiplier() const override { return 0.8; }
    int getNextThreshold() const override { return 300; }
    int getMonthlyCouponCount() const override { return 1; }
    void changeLevel(Customer *) override;
};
class GoldLevel : public MemberShipLevel
{
    string getLevelName() const override { return "Gold"; }
    double getDiscountRate() const override { return 0.1; }
    double getPointsMultiplier() const override { return 1.5; }
    double getDeliveryCostMultiplier() const override { return 0.5; }
    int getNextThreshold() const override { return 700; }
    int getMonthlyCouponCount() const override { return 1; }
    void changeLevel(Customer *) override;
};
class VIPLevel : public MemberShipLevel
{
    string getLevelName() const override { return "VIP"; }
    double getDiscountRate() const override { return 0.15; }
    double getPointsMultiplier() const override { return 2.0; }
    double getDeliveryCostMultiplier() const override { return 0.0; }
    int getNextThreshold() const override { return -1; }
    int getMonthlyCouponCount() const override { return 3; }
    void changeLevel(Customer *) override;
};