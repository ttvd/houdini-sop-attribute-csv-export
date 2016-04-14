#pragma once

#include <SOP/SOP_API.h>
#include <SOP/SOP_Node.h>
#include <UT/UT_OFStream.h>
#include <UT/UT_DeepString.h>

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
        bool getAttributeCSVNames(const GA_Attribute* attr, UT_Array<UT_DeepString>& attr_csv_names, bool skip_intrinsics) const;
        bool isSupportedAttribute(const GA_Attribute* attr, bool skip_intrinsics) const;

    protected:

        bool writeCSVAttributeNames(const UT_Array<UT_DeepString>& attr_csv_names, UT_OFStream& stream) const;
        void processAttributeValue(const GA_Attribute* attr, GA_Offset offset, GA_AttributeOwner owner, bool skip_intrinsics,
                UT_Array<UT_DeepString>& values) const;
};
