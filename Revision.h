#ifndef __FATE_REVISION
#define __FATE_REVISION

enum FateVariant {
    VARIENT_NONE;
};

class FateRevision {
   static double Revision() { return 0.99;}
   static FateVariant Variant() { return VARIENT_NONE;}
};
#endif

