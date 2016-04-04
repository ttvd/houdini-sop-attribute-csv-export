#pragma once

#include <SOP/SOP_API.h>
#include <SOP/SOP_Node.h>

class SOP_API SOP_AttributeCSVExport : public SOP_Node
{
    public:

        static OP_Node* myConstructor(OP_Network* network, const char* name, OP_Operator* op);
        static PRM_Template myTemplateList[];

        static const char* fileExtensionFilterString();

    protected:

        SOP_AttributeCSVExport(OP_Network* network, const char* name, OP_Operator* op);
        virtual ~SOP_AttributeCSVExport();

    protected:

        virtual const char* inputLabel(unsigned int idx) const;
        virtual OP_ERROR cookMySop(OP_Context& context);

    protected:

        bool getClassType(fpreal t, GA_AttributeOwner& attrib_owner) const;

    protected:

        bool getAttributeNames(UT_Array<UT_String>& attr_names) const;

    protected:

        bool exportCSVPoints(UT_IFStream& stream) const;
        bool exportCSVVertices(UT_IFStream& stream) const;
        bool exportCSVPrimitives(UT_IFStream& stream) const;
        bool exportCSVDetail(UT_IFStream& stream) const;
};
