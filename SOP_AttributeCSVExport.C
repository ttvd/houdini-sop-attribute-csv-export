#include "SOP_AttributeCSVExport.h"

#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <GEO/GEO_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <GU/GU_Detail.h>
#include <PRM/PRM_SpareData.h>

#define SOP_ATTRIBUTECSVEXPORT_CLASS "class"
#define SOP_ATTRIBUTECSVEXPORT_FILE "file"

static PRM_Name s_name_file(SOP_ATTRIBUTECSVEXPORT_FILE, "CSV File");
static PRM_Name s_name_class(SOP_ATTRIBUTECSVEXPORT_CLASS, "Class");

static PRM_Name s_name_class_types[] =
{
    PRM_Name("point", "Point"),
    PRM_Name("vertex", "Vertex"),
    PRM_Name("primitive", "Primitive"),
    PRM_Name("detail", "Detail"),
    PRM_Name(0),
};

static PRM_ChoiceList s_choicelist_class_type(PRM_CHOICELIST_SINGLE, s_name_class_types);

static PRM_SpareData s_spare_file_picker(PRM_SpareArgs() << PRM_SpareToken(PRM_SpareData::getFileChooserModeToken(),
    PRM_SpareData::getFileChooserModeValRead()) << PRM_SpareToken(PRM_SpareData::getFileChooserPatternToken(),
    SOP_AttributeCSVExport::fileExtensionFilterString()));

PRM_Template
SOP_AttributeCSVExport::myTemplateList[] = {
    PRM_Template(PRM_FILE, 1, &s_name_file, 0, 0, 0, 0, &s_spare_file_picker),
    PRM_Template(PRM_ORD, 1, &s_name_class, 0, &s_choicelist_class_type),
    PRM_Template()
};


OP_Node*
SOP_AttributeCSVExport::myConstructor(OP_Network* network, const char* name, OP_Operator* op)
{
    return new SOP_AttributeCSVExport(network, name, op);
}


const char*
SOP_AttributeCSVExport::fileExtensionFilterString()
{
    return "*.csv";
}


SOP_AttributeCSVExport::SOP_AttributeCSVExport(OP_Network* network, const char* name, OP_Operator* op) :
    SOP_Node(network, name, op)
{

}


SOP_AttributeCSVExport::~SOP_AttributeCSVExport()
{

}


OP_ERROR
SOP_AttributeCSVExport::cookMySop(OP_Context& context)
{
    fpreal t = context.getTime();
    GA_AttributeOwner attrib_owner = GA_ATTRIB_POINT;
    UT_String csv_filename;

    UT_Interrupt* boss = UTgetInterrupt();

    if(lockInputs(context) >= UT_ERROR_ABORT)
    {
        return error();
    }

    duplicatePointSource(0, context);

    if(!getClassType(t, attrib_owner))
    {
        UT_WorkBuffer buf;
        buf.sprintf("Unsupported attribute class type.");
        addError(SOP_MESSAGE, buf.buffer());

        unlockInputs();
        return error();
    }

    evalString(csv_filename, SOP_ATTRIBUTECSVEXPORT_FILE, 0, t);
    if(!csv_filename || !csv_filename.length())
    {
        UT_WorkBuffer buf;
        buf.sprintf("CSV file was not specified.");
        addError(SOP_MESSAGE, buf.buffer());

        unlockInputs();
        return error();
    }

    UT_OFStream stream(csv_filename, UT_OFStream::trunc, UT_IOS_ASCII);
    if(!stream)
    {
        UT_WorkBuffer buf;
        buf.sprintf("Error openening CSV file for writing.");
        addError(SOP_MESSAGE, buf.buffer());

        unlockInputs();
        return error();
    }

    UT_Array<UT_DeepString> csv_attr_names;
    const GA_Attribute* attr = nullptr;

    switch(attrib_owner)
    {
        default:
        case GA_ATTRIB_POINT:
        {
            GA_FOR_ALL_POINT_ATTRIBUTES(gdp, attr)
            {
                getAttributeCSVNames(attr, csv_attr_names);
            }

            break;
        }

        case GA_ATTRIB_VERTEX:
        {
            GA_FOR_ALL_VERTEX_ATTRIBUTES(gdp, attr)
            {
                getAttributeCSVNames(attr, csv_attr_names);
            }

            break;
        }

        case GA_ATTRIB_PRIMITIVE:
        {
            GA_FOR_ALL_PRIMITIVE_ATTRIBUTES(gdp, attr)
            {
                getAttributeCSVNames(attr, csv_attr_names);
            }

            break;
        }

        case GA_ATTRIB_DETAIL:
        {
            GA_FOR_ALL_DETAIL_ATTRIBUTES(gdp, attr)
            {
                getAttributeCSVNames(attr, csv_attr_names);
            }

            break;
        }
    }

    writeCSVAttributeNames(csv_attr_names, stream);

    switch(attrib_owner)
    {
        default:
        case GA_ATTRIB_POINT:
        {
            GA_Offset point_offset = 0;
            GA_FOR_ALL_PTOFF(gdp, point_offset)
            {
                GA_FOR_ALL_POINT_ATTRIBUTES(gdp, attr)
                {
                    GA_StorageClass storage_class = attr->getStorageClass();
                    int tuple_size = attr->getTupleSize();
                    UT_String attr_name(attr->getName());
                }
            }

            break;
        }

        case GA_ATTRIB_VERTEX:
        {
            break;
        }

        case GA_ATTRIB_PRIMITIVE:
        {
            break;
        }

        case GA_ATTRIB_DETAIL:
        {
            break;
        }
    }

    stream.close();
    unlockInputs();
    return error();
}


const char*
SOP_AttributeCSVExport::inputLabel(unsigned int idx) const
{
    return "Input geometry.";
}


bool
SOP_AttributeCSVExport::getClassType(fpreal t, GA_AttributeOwner& attrib_owner) const
{
    int class_type = evalInt(SOP_ATTRIBUTECSVEXPORT_CLASS, 0, t);

    switch(class_type)
    {
        case 0:
        {
            attrib_owner = GA_ATTRIB_POINT;
            return true;
        }

        case 1:
        {
            attrib_owner = GA_ATTRIB_VERTEX;
            return true;
        }

        case 2:
        {
            attrib_owner = GA_ATTRIB_PRIMITIVE;
            return true;
        }

        case 3:
        {
            attrib_owner = GA_ATTRIB_DETAIL;
            return true;
        }

        default:
        {
            break;
        }
    }

    return false;
}


bool
SOP_AttributeCSVExport::getAttributeCSVNames(const GA_Attribute* attr, UT_Array<UT_DeepString>& attr_csv_names) const
{
    int tuple_size = attr->getTupleSize();
    UT_DeepString attr_name(attr->getExportName());

    if(1 == tuple_size)
    {
        attr_csv_names.append(attr_name);
    }
    else
    {
        for(int idx = 0; idx < tuple_size; ++idx)
        {
            UT_DeepString attr_csv_name;
            attr_csv_name.sprintf("%s[%d]", attr_name.buffer(), idx);
            attr_csv_names.append(attr_csv_name);
        }
    }

    return true;
}


void
SOP_AttributeCSVExport::writeCSVAttributeNames(const UT_Array<UT_DeepString>& attr_csv_names, UT_OFStream& stream) const
{
    for(int idx = 0; idx < attr_csv_names.size(); ++idx)
    {
        const UT_DeepString& attr_name = attr_csv_names(idx);
        stream << attr_name;

        if(idx + 1 != attr_csv_names.size())
        {
            stream << ", ";
        }
    }

    stream << "\n";
}


void
newSopOperator(OP_OperatorTable* table)
{
    table->addOperator(new OP_Operator("attributecsvexport", "Attribute CSV Export", SOP_AttributeCSVExport::myConstructor,
        SOP_AttributeCSVExport::myTemplateList, 1, 1, 0));
}

