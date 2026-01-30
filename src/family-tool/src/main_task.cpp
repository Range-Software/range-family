#include <QSettings>
#include <QDate>
#include <QFileInfo>
#include <QDir>
#include <QLoggingCategory>

#include <rbl_arguments_parser.h>
#include <rbl_error.h>
#include <rbl_job_manager.h>
#include <rbl_logger.h>
#include <rbl_tool_task.h>

#include <rfl_tool_action.h>
#include <rfl_utils.h>

#include "main_task.h"

static QMap<QString, QString> argToPathMap
{
    {"sex","/person/sex"},
    {"first-name","/person/name/first-name"},
    {"middle-name","/person/name/middle-name"},
    {"last-name","/person/name/last-name"},
    {"maiden-name","/person/name/maiden-name"},
    {"birth-date","/person/birth/date"},
    {"birth-street","/person/birth/place/address/street"},
    {"birth-street-no","/person/birth/place/address/street-number"},
    {"birth-city","/person/birth/place/address/city"},
    {"birth-postal-code","/person/birth/place/address/postal-code"},
    {"birth-country","/person/birth/place/address/country"},
    {"birth-latitude","/person/birth/place/geo-location/latitude"},
    {"birth-longitude","/person/birth/place/geo-location/longitude"},
    {"birth-altitude","/person/birth/place/geo-location/altitude"},
    {"death-date","/person/death/date"},
    {"death-street","/person/death/place/address/street"},
    {"death-street-no","/person/death/place/address/street-number"},
    {"death-country","/person/death/place/address/country"},
    {"death-postal-code","/person/death/place/address/postal-code"},
    {"death-city","/person/death/place/address/city"},
    {"death-latitude","/person/death/place/geo-location/latitude"},
    {"death-longitude","/person/death/place/geo-location/longitude"},
    {"death-altitude","/person/death/place/geo-location/altitude"},
    {"partners","/relation/partners"},
    {"children","/relation/children"},
    {"begin-date","/relation/begin/date"},
    {"begin-street","/relation/begin/place/address/street"},
    {"begin-street-no","/relation/begin/place/address/street-number"},
    {"begin-city","/relation/begin/place/address/city"},
    {"begin-postal-code","/relation/begin/place/address/postal-code"},
    {"begin-country","/relation/begin/place/address/country"},
    {"begin-latitude","/relation/begin/place/geo-location/latitude"},
    {"begin-longitude","/relation/begin/place/geo-location/longitude"},
    {"begin-altitude","/relation/begin/place/geo-location/altitude"},
    {"end-date","/relation/end/date"},
    {"end-street","/relation/end/place/address/street"},
    {"end-street-no","/relation/end/place/address/street-number"},
    {"end-country","/relation/end/place/address/country"},
    {"end-postal-code","/relation/end/place/address/postal-code"},
    {"end-city","/relation/end/place/address/city"},
    {"end-latitude","/relation/end/place/geo-location/latitude"},
    {"end-longitude","/relation/end/place/geo-location/longitude"},
    {"end-altitude","/relation/end/place/geo-location/altitude"}
};

MainTask::MainTask(Application *application)
    : QObject(application)
    , application(application)
{
}

void MainTask::run()
{
    QSettings settings(RVendor::name(), RVendor::shortName());

    try
    {
        // Process command line arguments.
        QList<RArgumentOption> validOptions;

        validOptions.append(RArgumentOption("file",RArgumentOption::Path,QVariant(),"File name",RArgumentOption::Mandatory,false));
        validOptions.append(RArgumentOption("create-new",RArgumentOption::Switch,QVariant(),"Create new file (do not read existing)",RArgumentOption::File,false));
        validOptions.append(RArgumentOption("generate-dot-file",RArgumentOption::Switch,QVariant(),"Generate dot file (Graphviz format)",RArgumentOption::File,false));

        validOptions.append(RArgumentOption("log-file",RArgumentOption::Path,QVariant(),"Log file name",RArgumentOption::Logger,false));
        validOptions.append(RArgumentOption("log-debug",RArgumentOption::Switch,QVariant(),"Switch on debug log level",RArgumentOption::Logger,false));
        validOptions.append(RArgumentOption("log-trace",RArgumentOption::Switch,QVariant(),"Switch on trace log level",RArgumentOption::Logger,false));
        validOptions.append(RArgumentOption("log-ssl",RArgumentOption::Switch,QVariant(),"Enable Qt debug logging",RArgumentOption::Logger,false));
        validOptions.append(RArgumentOption("log-qt",RArgumentOption::Switch,QVariant(),"Enable Qt SSL debug logging",RArgumentOption::Logger,false));

        validOptions.append(RArgumentOption("add-person",RArgumentOption::Switch,QVariant(),"Add new person",RArgumentOption::Action,false));
        validOptions.append(RArgumentOption("remove-person",RArgumentOption::Switch,QVariant(),"Remove person identified by --person-id=<person-id>",RArgumentOption::Action,false));
        validOptions.append(RArgumentOption("modify-person",RArgumentOption::Switch,QVariant(),"Modify person identified by --person-id=<person-id>",RArgumentOption::Action,false));
        validOptions.append(RArgumentOption("print-person",RArgumentOption::Switch,QVariant(),"Print person identified by --person-id=<person-id>",RArgumentOption::Action,false));
        validOptions.append(RArgumentOption("list-persons",RArgumentOption::Switch,QVariant(),"List all persons",RArgumentOption::Action,false));

        validOptions.append(RArgumentOption("sex",RArgumentOption::String,QVariant(),"Sex at birth",RArgumentOption::Optional,false));

        validOptions.append(RArgumentOption("first-name",RArgumentOption::String,QVariant(),"First name",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("middle-name",RArgumentOption::String,QVariant(),"Middle name",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("last-name",RArgumentOption::String,QVariant(),"Last name",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("maiden-name",RArgumentOption::String,QVariant(),"Maiden (birth) name",RArgumentOption::Optional,false));

        validOptions.append(RArgumentOption("birth-date",RArgumentOption::Date,QVariant(),"Person birth date",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("birth-street",RArgumentOption::String,QVariant(),"Person birth street",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("birth-street-no",RArgumentOption::String,QVariant(),"Person birth street number",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("birth-city",RArgumentOption::String,QVariant(),"Person birth city",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("birth-postal-code",RArgumentOption::String,QVariant(),"Person birth postal code",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("birth-country",RArgumentOption::String,QVariant(),"Person birth country",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("birth-latitude",RArgumentOption::Real,QVariant(0.0),"Person birth latitude",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("birth-longitude",RArgumentOption::Real,QVariant(0.0),"Person birth longitude",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("birth-altitude",RArgumentOption::Real,QVariant(0.0),"Person birth altitude",RArgumentOption::Optional,false));

        validOptions.append(RArgumentOption("death-date",RArgumentOption::Date,QVariant(),"Person death date",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("death-street",RArgumentOption::String,QVariant(),"Person death street",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("death-street-no",RArgumentOption::String,QVariant(),"Person death street number",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("death-city",RArgumentOption::String,QVariant(),"Person death city",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("death-postal-code",RArgumentOption::String,QVariant(),"Person death postal code",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("death-country",RArgumentOption::String,QVariant(),"Person death country",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("death-latitude",RArgumentOption::Real,QVariant(0.0),"Person death latitude",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("death-longitude",RArgumentOption::Real,QVariant(0.0),"Person death longitude",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("death-altitude",RArgumentOption::Real,QVariant(0.0),"Person death altitude",RArgumentOption::Optional,false));

        validOptions.append(RArgumentOption("add-relation",RArgumentOption::Switch,QVariant(),"Add new relation",RArgumentOption::Action,false));
        validOptions.append(RArgumentOption("remove-relation",RArgumentOption::Switch,QVariant(),"Remove relation identified by --relation-id=<relation-id>",RArgumentOption::Action,false));
        validOptions.append(RArgumentOption("modify-relation",RArgumentOption::Switch,QVariant(),"Modify relation identified by --relation-id=<relation-id>",RArgumentOption::Action,false));
        validOptions.append(RArgumentOption("print-relation",RArgumentOption::Switch,QVariant(),"Print relation identified by --relation-id=<relation-id>",RArgumentOption::Action,false));
        validOptions.append(RArgumentOption("list-relations",RArgumentOption::Switch,QVariant(),"List all persons",RArgumentOption::Action,false));

        validOptions.append(RArgumentOption("person-id",RArgumentOption::String,QVariant(),"Person ID",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("partners",RArgumentOption::StringList,QVariant(),"Comma separated list of pratner IDs",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("children",RArgumentOption::StringList,QVariant(),"Comma separated list of children IDs",RArgumentOption::Optional,false));

        validOptions.append(RArgumentOption("relation-id",RArgumentOption::String,QVariant(),"Relation ID",RArgumentOption::Optional,false));

        validOptions.append(RArgumentOption("begin-date",RArgumentOption::Date,QVariant(),"Relation begin date",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("begin-street",RArgumentOption::String,QVariant(),"Relation begin street",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("begin-street-no",RArgumentOption::String,QVariant(),"Relation begin street number",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("begin-city",RArgumentOption::String,QVariant(),"Relation begin city",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("begin-postal-code",RArgumentOption::String,QVariant(),"Relation begin postal code",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("begin-country",RArgumentOption::String,QVariant(),"Relation begin country",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("begin-latitude",RArgumentOption::Real,QVariant(0.0),"Relation begin latitude",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("begin-longitude",RArgumentOption::Real,QVariant(0.0),"Relation begin longitude",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("begin-altitude",RArgumentOption::Real,QVariant(0.0),"Relation begin altitude",RArgumentOption::Optional,false));

        validOptions.append(RArgumentOption("end-date",RArgumentOption::Date,QVariant(),"Relation end date",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("end-street",RArgumentOption::String,QVariant(),"Relation end street",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("end-street-no",RArgumentOption::String,QVariant(),"Relation end street number",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("end-city",RArgumentOption::String,QVariant(),"Relation end city",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("end-postal-code",RArgumentOption::String,QVariant(),"Relation end postal code",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("end-country",RArgumentOption::String,QVariant(),"Relation end country",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("end-latitude",RArgumentOption::Real,QVariant(0.0),"Relation begin latitude",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("end-longitude",RArgumentOption::Real,QVariant(0.0),"Relation begin longitude",RArgumentOption::Optional,false));
        validOptions.append(RArgumentOption("end-altitude",RArgumentOption::Real,QVariant(0.0),"Relation begin altitude",RArgumentOption::Optional,false));

        validOptions.append(RArgumentOption("sort-path",RArgumentOption::String,QVariant(),"Sort path (e.g. 'last-name' or 'person/name/last-name')",RArgumentOption::Optional,false));

        RArgumentsParser argumentsParser(Application::arguments(),validOptions,false);

        if (argumentsParser.isSet("help"))
        {
            argumentsParser.printHelp();
            this->application->exit(0);
            return;
        }

        if (argumentsParser.isSet("version"))
        {
            argumentsParser.printVersion();
            this->application->exit(0);
            return;
        }

        if (argumentsParser.isSet("log-debug"))
        {
            RLogger::getInstance().setLevel(R_LOG_LEVEL_DEBUG);
        }
        if (argumentsParser.isSet("log-trace"))
        {
            RLogger::getInstance().setLevel(R_LOG_LEVEL_TRACE);
        }
        if (argumentsParser.isSet("log-qt") ||
            argumentsParser.isSet("log-ssl"))
        {
            RLogger::installQtMessageHandler();
            RLogger::debug("Qt message handler installed\n");

            if (argumentsParser.isSet("log-qt"))
            {
                QLoggingCategory::setFilterRules("qt.*=true");
                RLogger::debug("All Qt logging enabled\n");
            }
            else if (argumentsParser.isSet("log-ssl"))
            {
                QLoggingCategory::setFilterRules(
                    "qt.network.ssl.debug=true\n"
                    "qt.network.ssl.info=true\n"
                    "qt.network.ssl.warning=true\n"
                    "qt.tlsbackend.ossl.debug=true\n"
                    "qt.tlsbackend.ossl.info=true\n"
                    "qt.tlsbackend.ossl.warning=true"
                    );
                RLogger::debug("SSL logging enabled\n");
            }
        }
        if (argumentsParser.isSet("log-file"))
        {
            RLogger::getInstance().setFile(argumentsParser.getValue("log-file").toString());
        }

        QString inXmlFileName(argumentsParser.getValue("file").toString());
        QString outXmlFileName(argumentsParser.getValue("file").toString());

        RToolInput toolInput;

        if (!argumentsParser.isSet("create-new"))
        {
            // Read XML file action
            this->application->getTree()->readXmlFile(inXmlFileName);
            RLogger::info("Tree has been successfully loaded from file \'%s\'.\n", inXmlFileName.toUtf8().constData());
        }

        QString sortPath;
        if (argumentsParser.isSet("sort-path"))
        {
            sortPath = argumentsParser.getValue("sort-path").toString();
            sortPath = argToPathMap.value(sortPath,sortPath);
        }

        bool writeXml(false);

        // PERSON

        QUuid personId;
        FPerson person;

        if (argumentsParser.isSet("person-id"))
        {
            personId = QUuid::fromString(argumentsParser.getValue("person-id").toString());
        }

        if (argumentsParser.isSet("modify-person"))
        {
            person = this->application->getTree()->findPerson(personId);
        }

        // Person sex

        QString sex;
        if (argumentsParser.isSet("sex"))
        {
            sex = argumentsParser.getValue("sex").toString();
            person.setSex(sex);
        }

        // Person name

        FName name = person.getName();
        if (argumentsParser.isSet("first-name"))
        {
            name.setFirstName(argumentsParser.getValue("first-name").toString());
        }
        if (argumentsParser.isSet("middle-name"))
        {
            name.setMiddleName(argumentsParser.getValue("middle-name").toString());
        }
        if (argumentsParser.isSet("last-name"))
        {
            name.setLastName(argumentsParser.getValue("last-name").toString());
        }
        if (argumentsParser.isSet("maiden-name"))
        {
            name.setMaidenName(argumentsParser.getValue("maiden-name").toString());
        }
        person.setName(name);

        // Person birth

        FEvent birth = person.getBirth();
        if (argumentsParser.isSet("birth-date"))
        {
            QDate birthDate(QDate::fromString(argumentsParser.getValue("birth-date").toString(),"dd.MM.yyyy"));
            birth.setDate(FDate(birthDate.day(),birthDate.month(),birthDate.year()));
        }
        FPlace birthPlace = birth.getPlace();
        FAddress birthAddress = birthPlace.getAddress();
        if (argumentsParser.isSet("birth-street"))
        {
            birthAddress.setStreet(argumentsParser.getValue("birth-street").toString());
        }
        if (argumentsParser.isSet("birth-street-no"))
        {
            birthAddress.setStreetNumber(argumentsParser.getValue("birth-street-no").toString());
        }
        if (argumentsParser.isSet("birth-city"))
        {
            birthAddress.setCity(argumentsParser.getValue("birth-city").toString());
        }
        if (argumentsParser.isSet("birth-postal-code"))
        {
            birthAddress.setPostalCode(argumentsParser.getValue("birth-postal-code").toString());
        }
        if (argumentsParser.isSet("birth-country"))
        {
            birthAddress.setCountry(argumentsParser.getValue("birth-country").toString());
        }
        birthPlace.setAddress(birthAddress);
        FGeoCoordinate birthCoordinates = birthPlace.getGeoCoordinates();
        if (argumentsParser.isSet("birth-latitude"))
        {
            birthCoordinates.setLatitude(argumentsParser.getValue("birth-latitude").toDouble());
        }
        if (argumentsParser.isSet("birth-longitude"))
        {
            birthCoordinates.setLongitude(argumentsParser.getValue("birth-longitude").toDouble());
        }
        if (argumentsParser.isSet("birth-altitude"))
        {
            birthCoordinates.setAltitude(argumentsParser.getValue("birth-altitude").toDouble());
        }
        birthPlace.setGeoCoordinates(birthCoordinates);
        birth.setPlace(birthPlace);
        person.setBirth(birth);

        // Person death

        FEvent death = person.getDeath();
        if (argumentsParser.isSet("death-date"))
        {
            QDate deathDate(QDate::fromString(argumentsParser.getValue("death-date").toString(),"dd.MM.yyyy"));
            death.setDate(FDate(deathDate.day(),deathDate.month(),deathDate.year()));
        }
        FPlace deathPlace = death.getPlace();
        FAddress deathAddress = deathPlace.getAddress();
        if (argumentsParser.isSet("death-street"))
        {
            deathAddress.setStreet(argumentsParser.getValue("death-street").toString());
        }
        if (argumentsParser.isSet("death-street-no"))
        {
            deathAddress.setStreetNumber(argumentsParser.getValue("death-street-no").toString());
        }
        if (argumentsParser.isSet("death-city"))
        {
            deathAddress.setCity(argumentsParser.getValue("death-city").toString());
        }
        if (argumentsParser.isSet("death-postal-code"))
        {
            deathAddress.setPostalCode(argumentsParser.getValue("death-postal-code").toString());
        }
        if (argumentsParser.isSet("death-country"))
        {
            deathAddress.setCountry(argumentsParser.getValue("death-country").toString());
        }
        deathPlace.setAddress(deathAddress);
        FGeoCoordinate deathCoordinates = deathPlace.getGeoCoordinates();
        if (argumentsParser.isSet("death-latitude"))
        {
            deathCoordinates.setLatitude(argumentsParser.getValue("death-latitude").toDouble());
        }
        if (argumentsParser.isSet("death-longitude"))
        {
            deathCoordinates.setLongitude(argumentsParser.getValue("death-longitude").toDouble());
        }
        if (argumentsParser.isSet("death-altitude"))
        {
            deathCoordinates.setAltitude(argumentsParser.getValue("death-altitude").toDouble());
        }
        deathPlace.setGeoCoordinates(deathCoordinates);
        death.setPlace(deathPlace);
        person.setDeath(death);

        if (argumentsParser.isSet("add-person"))
        {
            person.setId(FTree::generateId());
            toolInput.addAction(FToolAction::addPerson(this->application->getTree(),person));
            writeXml = true;
        }
        if (argumentsParser.isSet("modify-person"))
        {
            toolInput.addAction(FToolAction::modifyPerson(this->application->getTree(),person));
            writeXml = true;
        }

        // RELATION

        QUuid relationId;
        FRelation relation;

        if (argumentsParser.isSet("relation-id"))
        {
            relationId = QUuid::fromString(argumentsParser.getValue("relation-id").toString());
        }

        if (argumentsParser.isSet("modify-relation"))
        {
            relation = this->application->getTree()->findRelation(relationId);
        }

        // Relation partners

        QList<QUuid> partnersIds = relation.getPartners();
        if (argumentsParser.isSet("partners"))
        {
            partnersIds.clear();
            foreach (const QString &partnerIdString, argumentsParser.getValue("partners").toString().split(QLatin1Char(','), Qt::SkipEmptyParts))
            {
                partnersIds.append(QUuid(partnerIdString));
            }
            if (partnersIds.size() == 0)
            {
                // Add NULL uuid to force list erasure
                partnersIds.append(QUuid());
            }
        }
        relation.setPartners(partnersIds);

        // Relation children

        QList<QUuid> childrenIds = relation.getChildren();
        if (argumentsParser.isSet("children"))
        {
            foreach (const QString &childrenIdString, argumentsParser.getValue("children").toString().split(QLatin1Char(','), Qt::SkipEmptyParts))
            {
                childrenIds.append(QUuid(childrenIdString));
            }
            if (childrenIds.size() == 0)
            {
                // Add NULL uuid to force list erasure
                childrenIds.append(QUuid());
            }
        }
        relation.setChildren(childrenIds);

        // Relation begin

        FEvent begin = relation.getBegin();
        if (argumentsParser.isSet("begin-date"))
        {
            QDate beginDate(QDate::fromString(argumentsParser.getValue("begin-date").toString(),"dd.MM.yyyy"));
            begin.setDate(FDate(beginDate.day(),beginDate.month(),beginDate.year()));
        }
        FPlace beginPlace = begin.getPlace();
        FAddress beginAddress = beginPlace.getAddress();
        if (argumentsParser.isSet("begin-street"))
        {
            beginAddress.setStreet(argumentsParser.getValue("begin-street").toString());
        }
        if (argumentsParser.isSet("begin-street-no"))
        {
            beginAddress.setStreetNumber(argumentsParser.getValue("begin-street-no").toString());
        }
        if (argumentsParser.isSet("begin-city"))
        {
            beginAddress.setCity(argumentsParser.getValue("begin-city").toString());
        }
        if (argumentsParser.isSet("begin-postal-code"))
        {
            beginAddress.setPostalCode(argumentsParser.getValue("begin-postal-code").toString());
        }
        if (argumentsParser.isSet("begin-country"))
        {
            beginAddress.setCountry(argumentsParser.getValue("begin-country").toString());
        }
        beginPlace.setAddress(beginAddress);
        FGeoCoordinate beginCoordinates = beginPlace.getGeoCoordinates();
        if (argumentsParser.isSet("begin-latitude"))
        {
            beginCoordinates.setLatitude(argumentsParser.getValue("begin-latitude").toDouble());
        }
        if (argumentsParser.isSet("begin-longitude"))
        {
            beginCoordinates.setLongitude(argumentsParser.getValue("begin-longitude").toDouble());
        }
        if (argumentsParser.isSet("begin-altitude"))
        {
            beginCoordinates.setAltitude(argumentsParser.getValue("begin-altitude").toDouble());
        }
        beginPlace.setGeoCoordinates(beginCoordinates);
        begin.setPlace(beginPlace);
        relation.setBegin(begin);

        // Relation end

        FEvent end = relation.getEnd();
        if (argumentsParser.isSet("end-date"))
        {
            QDate endDate(QDate::fromString(argumentsParser.getValue("end-date").toString(),"dd.MM.yyyy"));
            end.setDate(FDate(endDate.day(),endDate.month(),endDate.year()));
        }
        FPlace endPlace = end.getPlace();
        FAddress endAddress = endPlace.getAddress();
        if (argumentsParser.isSet("end-street"))
        {
            endAddress.setStreet(argumentsParser.getValue("end-street").toString());
        }
        if (argumentsParser.isSet("end-street-no"))
        {
            endAddress.setStreetNumber(argumentsParser.getValue("end-street-no").toString());
        }
        if (argumentsParser.isSet("end-city"))
        {
            endAddress.setCity(argumentsParser.getValue("end-city").toString());
        }
        if (argumentsParser.isSet("end-postal-code"))
        {
            endAddress.setPostalCode(argumentsParser.getValue("end-postal-code").toString());
        }
        if (argumentsParser.isSet("end-country"))
        {
            endAddress.setCountry(argumentsParser.getValue("end-country").toString());
        }
        endPlace.setAddress(endAddress);
        FGeoCoordinate endCoordinates = endPlace.getGeoCoordinates();
        if (argumentsParser.isSet("end-latitude"))
        {
            endCoordinates.setLatitude(argumentsParser.getValue("end-latitude").toDouble());
        }
        if (argumentsParser.isSet("end-longitude"))
        {
            endCoordinates.setLongitude(argumentsParser.getValue("end-longitude").toDouble());
        }
        if (argumentsParser.isSet("end-altitude"))
        {
            endCoordinates.setAltitude(argumentsParser.getValue("end-altitude").toDouble());
        }
        endPlace.setGeoCoordinates(endCoordinates);
        end.setPlace(endPlace);
        relation.setEnd(end);

        if (argumentsParser.isSet("remove-person"))
        {
            toolInput.addAction(FToolAction::removePerson(this->application->getTree(),personId));
            writeXml = true;
        }
        if (argumentsParser.isSet("add-relation"))
        {
            relation.setId(FTree::generateId());
            relation.setChildren(childrenIds);
            relation.setBegin(begin);
            relation.setEnd(end);

            toolInput.addAction(FToolAction::addRelation(this->application->getTree(),relation));
            writeXml = true;
        }
        if (argumentsParser.isSet("modify-relation"))
        {
            relation.setChildren(childrenIds);
            relation.setBegin(begin);
            relation.setEnd(end);

            toolInput.addAction(FToolAction::modifyRelation(this->application->getTree(),relation));
            writeXml = true;
        }
        if (argumentsParser.isSet("remove-relation"))
        {
            toolInput.addAction(FToolAction::removeRelation(this->application->getTree(),relationId));
            writeXml = true;
        }
        if (argumentsParser.isSet("list-persons"))
        {
            toolInput.addAction(FToolAction::listPersons(this->application->getTree(),sortPath));
        }
        if (argumentsParser.isSet("list-relations"))
        {
            toolInput.addAction(FToolAction::listRelations(this->application->getTree(),sortPath));
        }
        if (argumentsParser.isSet("print-person"))
        {
            toolInput.addAction(FToolAction::printPerson(this->application->getTree(),personId));
        }
        if (argumentsParser.isSet("print-relation"))
        {
            toolInput.addAction(FToolAction::printRelation(this->application->getTree(),relationId));
        }
        if (argumentsParser.isSet("generate-dot-file"))
        {
            QFileInfo fi(inXmlFileName);
            QString outDotFileName = fi.path() + QDir::separator() + fi.completeBaseName() + ".dot";
            toolInput.addAction(FToolAction::generateDot(this->application->getTree(),outDotFileName));
        }

        if (writeXml)
        {
            // Write XML file action
            toolInput.addAction(FToolAction::writeTreeFile(this->application->getTree(),outXmlFileName));
        }

        // Start tool.
        RToolTask *toolTask = new RToolTask(toolInput);
        RJobManager::getInstance().submit(toolTask);

        QObject::connect(toolTask, &RToolTask::failed, this, &MainTask::taskFailed);
        QObject::connect(&RJobManager::getInstance(), &RJobManager::jobFinished, this->application, &Application::quit);
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to start tool. %s\n",error.getMessage().toUtf8().constData());
        this->application->exit(1);
    }
}

void MainTask::taskFailed()
{
    this->application->exit(1);
}
