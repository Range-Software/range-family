#include <QVBoxLayout>
#include <QApplication>

#include "person_details_widget.h"

#define _FP_TR(qstr) qApp->translate("FPerson",qstr.toUtf8().constData())

PersonDetailsWidget::PersonDetailsWidget(const FTree *tree, QWidget *parent)
    : QWidget(parent)
    , tree(tree)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->htmlViewer = new RHtmlViewer;
    mainLayout->addWidget(this->htmlViewer);

    QObject::connect(tree,&FTree::personChanged,this,&PersonDetailsWidget::onPersonChanged);
}

void PersonDetailsWidget::setPersons(const QList<QUuid> &personsIds)
{
    this->personsIds = personsIds;
    this->populate();
}

void PersonDetailsWidget::populate()
{
    QString htmlContent;

    htmlContent += "<!DOCTYPE html>";
    htmlContent += "<html lang=\"en\">";
    htmlContent += "<head>";
    htmlContent += "<title>" + tr("Person details") + "</title>";
    htmlContent += "<style type=\"text/css\">";
    htmlContent += "table, th, td {\
                      border: 1px solid darkgrey; \
                      border-collapse: collapse; \
                      background: whitesmoke; \
                      text-align: left; \
                      color: black; \
                    }";
    htmlContent += "</style>";
    htmlContent += "</head>";
    htmlContent += "<body>";

    foreach (QUuid personId, this->personsIds)
    {
        FPerson person = this->tree->findPerson(personId);
        htmlContent += "<table width=\"100%\">";
        htmlContent += "<tr><th colspan=\"2\" align=\"center\" style=\"background: lightgrey; border: 1px solid darkgrey;\">" + person.getName().toString() + "</th></tr>";
        htmlContent += "<tr><th>" + tr("Sex")  + "</th><td width=\"80%\">" + _FP_TR(FPerson::sexKeyToDesc(person.getSex())) + "</td></tr>";
        QString bornDateString = QLocale().toString(person.getBirth().getDate().toDate());
        QString bornPlaceString = person.getBirth().getPlace().getAddress().toString();
        QString bornSeparator = !bornDateString.isEmpty() && !bornPlaceString.isEmpty() ? ", " : QString();
        htmlContent += "<tr><th>" + tr("Born") + "</th><td>" + bornDateString + bornSeparator + bornPlaceString + "</td></tr>";
        QString deathDateString = QLocale().toString(person.getBirth().getDate().toDate());
        QString deathPlaceString = person.getBirth().getPlace().getAddress().toString();
        QString deathSeparator = !deathDateString.isEmpty() && !deathPlaceString.isEmpty() ? ", " : QString();
        htmlContent += "<tr><th>" + tr("Died") + "</th><td>" + deathDateString + deathSeparator + deathPlaceString + "</td></tr>";

        QList<QUuid> partnerRelations = this->tree->findPersonPartnerRelations(personId);
        QList<QUuid> childRelations = this->tree->findPersonChildRelations(personId);

        if (childRelations.size() > 0)
        {
            foreach (QUuid relationId, childRelations) {
                FRelation relation = this->tree->findRelation(relationId);

                htmlContent += "<tr><th colspan=\"2\" align=\"center\">" + tr("Relation as a child") + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]</th>";
                htmlContent += "<tr><th>" + tr("Parents") + "</th><td>";
                htmlContent += "<ul>";
                foreach (QUuid parentId, this->tree->sortPersons(relation.getPartners())) {
                    FPerson parent = this->tree->findPerson(parentId);
                    htmlContent += "<li>";
                    htmlContent += parent.getName().toString() + " [" + QLocale().toString(parent.getBirth().getDate().toDate()) + " - " + QLocale().toString(parent.getDeath().getDate().toDate()) + "]";
                    htmlContent += "</li>";
                }
                htmlContent += "</ul>";
                htmlContent += "</td>";
                htmlContent += "</tr>";

                htmlContent += "<tr><th>" + tr("Siblings") + "</th><td>";
                htmlContent += "<ul>";
                foreach (QUuid childId, this->tree->sortPersons(relation.getChildren())) {
                    if (childId == personId)
                    {
                        continue;
                    }
                    FPerson child = this->tree->findPerson(childId);
                    htmlContent += "<li>";
                    htmlContent += child.getName().toString() + " [" + QLocale().toString(child.getBirth().getDate().toDate()) + " - " + QLocale().toString(child.getDeath().getDate().toDate()) + "]";
                    htmlContent += "</li>";
                }
                htmlContent += "</ul>";
                htmlContent += "</td>";
                htmlContent += "</tr>";
            }
        }

        if (partnerRelations.size() > 0)
        {
            foreach (QUuid relationId, partnerRelations) {
                FRelation relation = this->tree->findRelation(relationId);

                htmlContent += "<tr><th colspan=\"2\" align=\"center\">" + tr("Relation as a partner") + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]</th>";
                htmlContent += "<tr><th>" + tr("Partner") + "</th><td>";
                htmlContent += "<ul>";
                foreach (QUuid partnerId, relation.getPartners()) {
                    if (partnerId == personId)
                    {
                        continue;
                    }
                    FPerson partner = this->tree->findPerson(partnerId);
                    htmlContent += "<li>";
                    htmlContent += partner.getName().toString() + " [" + QLocale().toString(partner.getBirth().getDate().toDate()) + " - " + QLocale().toString(partner.getDeath().getDate().toDate()) + "]";
                    htmlContent += "</li>";
                }
                htmlContent += "</ul>";
                htmlContent += "</td>";
                htmlContent += "</tr>";

                htmlContent += "<tr><th>" + tr("Children") + "</th><td>";
                htmlContent += "<ul>";
                foreach (QUuid childId, relation.getChildren()) {
                    FPerson child = this->tree->findPerson(childId);
                    htmlContent += "<li>";
                    htmlContent += child.getName().toString() + " [" + QLocale().toString(child.getBirth().getDate().toDate()) + " - " + QLocale().toString(child.getDeath().getDate().toDate()) + "]";
                    htmlContent += "</li>";
                }
                htmlContent += "</ul>";
                htmlContent += "</td>";
                htmlContent += "</tr>";
            }
        }

        htmlContent += "<tr><th colspan=\"2\" align=\"center\">" + tr("Additional information") + "</th></tr>";
        htmlContent += "<tr><td colspan=\"2\"><p style=\"white-space: pre-wrap;\">" + person.getText().toHtmlEscaped() + "</p></td></tr>";
        htmlContent += "<tr><td colspan=\"2\" align=\"left\">";
        htmlContent += "<table style=\"border: 0px\">";
        htmlContent += "<tr>";
        htmlContent += "<td style=\"border: 0px\">";
        htmlContent += "<img src=\"data:image/png;base64," + person.getPicture().getData().toBase64() + "\">";
        htmlContent += "</td>";
        htmlContent += "<td style=\"border: 0px\">";
        htmlContent += "<strong>" + person.getPicture().getTitle() + "</strong><br/>" + person.getPicture().getDescription();
        htmlContent += "</td>";
        htmlContent += "</tr>";
        htmlContent += "</table>";
        htmlContent += "</td></tr>";

        htmlContent += "</table>";
        htmlContent += "<br/>";
    }
    htmlContent += "</body>";
    htmlContent += "</html>";

    this->htmlViewer->setHtml(htmlContent);

}

void PersonDetailsWidget::onPersonChanged(const FPerson &person)
{
    if (personsIds.contains(person.getId()))
    {
        this->populate();
    }
}
