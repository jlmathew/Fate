#include "../NormalizeValue.h"
#include <iostream>
#include <ostream>

int main()
{
    Normalize<int> *p;
    NormalizeValue<int> p0;

    NormalRanked<int> n;
    NormalMatch<int> m;
    GeometricRanked<int> g;
	    StepRanked<int> sr;
    std::cout << "NormalizeValue:" << "\n";
    n.InsertValue(3);
    n.InsertValue(7);
    n.InsertValue(10);
    n.DeleteValue(10);
    std::cout << "expect 0, received " << n.EvaluateValue(3) << "\n";
    std::cout << "expect 1, received " << n.EvaluateValue(7) << "\n";
    n.AllowZeroValues(false);
    std::cout << "expect .2, received " << n.EvaluateValue(3) << "\n";
    std::cout << "expect .8, received " << n.EvaluateValue(6) << "\n";

    std::cout << "\nNormalMatch:\n";
    m.InsertValue(3); 
    m.InsertValue(4); 
    m.InsertValue(5); 
    RangeData<int> rd(4,5);
    m.InsertMatchValue(rd);
    std::cout << "expect 0, received " << m.EvaluateValue(2)<<"\n";
    std::cout << "expect 0, received " << m.EvaluateValue(3)<<"\n";
    std::cout << "expect 1.0, received " << m.EvaluateValue(4)<<"\n";
    std::cout << "expect 1.0, received " << m.EvaluateValue(5)<<"\n";
    
    std::cout << "\nGeometricMatch:\n";
    std::cout << "expect 1, received " << g.EvaluateValue(0)<<"\n";
    std::cout << "expect 1, received " << g.EvaluateValue(1)<<"\n";
    std::cout << "expect 0.1, received " << g.EvaluateValue(10)<<"\n";
    g.SetInvert(true);
    std::cout << "expect 0.9, received " << g.EvaluateValue(10)<<"\n";

    std::cout << "\nStepMatch:\n";
    sr.InsertValue(10);
    sr.InsertValue(1);
    sr.InsertValue(5);
    sr.InsertValue(2);
    sr.InsertValue(7);
    std::cout << "Expect 1, received " << sr.Evaluate(10) << "\n";
    std::cout << "Expect 0.8, received " << sr.Evaluate(7) << "\n";
    std::cout << "Expect 0.6, received " << sr.Evaluate(5) << "\n";
    std::cout << "Expect 0.4, received " << sr.Evaluate(2) << "\n";
    std::cout << "Expect 0.2, received " << sr.Evaluate(1) << "\n";
    std::cout << "Expect 0, received " << sr.Evaluate(4) << "\n";
}
