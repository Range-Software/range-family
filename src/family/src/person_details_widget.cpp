#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyleHints>

#include "person_details_widget.h"

#define _FP_TR(qstr) qApp->translate("FPerson",qstr.toUtf8().constData())

PersonDetailsWidget::PersonDetailsWidget(const FTree *tree, QWidget *parent)
    : QWidget(parent)
    , tree(tree)
    , darkMode{QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark}
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
    this->htmlViewer->setHtml(this->generateHtml());
    // this->htmlViewer->setMarkdown(this->generateMarkdown());
}

QString PersonDetailsWidget::generateMarkdown() const
{
    QString content;

    bool isFirst = true;

    foreach (QUuid personId, this->personsIds)
    {
        if (!isFirst)
        {
            content += "\n---\n\n";
        }
        isFirst = false;

        FPerson person = this->tree->findPerson(personId);

        content += "## " + person.getName().toString() + '\n';
        content += '\n';
        if (!person.getPicture().isEmpty())
        {
            if (!person.getPicture().getTitle().isEmpty())
            {
                content += "**" + person.getPicture().getTitle() + "** " + '\n';
            }
            content += "![Personal photo](data:image/png;base64," + person.getPicture().getData().toBase64() + ")" + '\n';
            if (!person.getPicture().getDescription().isEmpty())
            {
                content += "*" + person.getPicture().getDescription() + "* " + '\n';
            }
        }
        content += '\n';
        content += "- " + tr("Sex") + ": " + _FP_TR(FPerson::sexKeyToDesc(person.getSex())) + '\n';

        QString bornDateString = QLocale().toString(person.getBirth().getDate().toDate());
        QString bornPlaceString = person.getBirth().getPlace().getAddress().toString();
        QString bornSeparator = !bornDateString.isEmpty() && !bornPlaceString.isEmpty() ? ", " : QString();

        content += "- " + tr("Born") + ": " + bornDateString + bornSeparator + bornPlaceString + '\n';

        QString deathDateString = QLocale().toString(person.getBirth().getDate().toDate());
        QString deathPlaceString = person.getBirth().getPlace().getAddress().toString();
        QString deathSeparator = !deathDateString.isEmpty() && !deathPlaceString.isEmpty() ? ", " : QString();

        content += "- " + tr("Died") + ": " + deathDateString + deathSeparator + deathPlaceString + '\n';

        QList<QUuid> partnerRelations = this->tree->findPersonPartnerRelations(personId);
        QList<QUuid> childRelations = this->tree->findPersonChildRelations(personId);

        if (childRelations.size() > 0)
        {
            content += "### " + tr("Parents and siblings") + '\n';
            content += '\n';

            foreach (QUuid relationId, childRelations) {
                FRelation relation = this->tree->findRelation(relationId);

                content += "#### " + tr("Parents")  + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]" + '\n';
                content += '\n';
                foreach (QUuid parentId, this->tree->sortPersons(relation.getPartners())) {
                    FPerson parent = this->tree->findPerson(parentId);
                    content += "- " + parent.getName().toString() + " [" + QLocale().toString(parent.getBirth().getDate().toDate()) + " - " + QLocale().toString(parent.getDeath().getDate().toDate()) + "]" + '\n';
                }
                content += '\n';

                content += "#### " + tr("Siblings") + '\n';
                content += '\n';
                foreach (QUuid childId, this->tree->sortPersons(relation.getChildren())) {
                    if (childId == personId)
                    {
                        continue;
                    }
                    FPerson child = this->tree->findPerson(childId);
                    content += "- " + child.getName().toString() + " [" + QLocale().toString(child.getBirth().getDate().toDate()) + " - " + QLocale().toString(child.getDeath().getDate().toDate()) + "]" + '\n';
                }
                content += '\n';
            }
        }

        if (partnerRelations.size() > 0)
        {
            content += "### " + tr("Partner and children") + '\n';
            content += '\n';
            foreach (QUuid relationId, partnerRelations) {
                FRelation relation = this->tree->findRelation(relationId);

                content += "#### " + tr("Partner") + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]" + '\n';
                content += '\n';
                foreach (QUuid partnerId, relation.getPartners()) {
                    if (partnerId == personId)
                    {
                        continue;
                    }
                    FPerson partner = this->tree->findPerson(partnerId);
                    content += "- " + partner.getName().toString() + " [" + QLocale().toString(partner.getBirth().getDate().toDate()) + " - " + QLocale().toString(partner.getDeath().getDate().toDate()) + "]" + '\n';
                }
                content += '\n';

                content += "#### " + tr("Children") + '\n';
                content += '\n';
                foreach (QUuid childId, relation.getChildren()) {
                    FPerson child = this->tree->findPerson(childId);
                    content += "- " + child.getName().toString() + " [" + QLocale().toString(child.getBirth().getDate().toDate()) + " - " + QLocale().toString(child.getDeath().getDate().toDate()) + "]" + '\n';
                }
                content += '\n';
            }
        }

        if (!person.getText().isEmpty())
        {
            content += "### " + tr("Additional information") + '\n';
            content += '\n';
            content += person.getText().toHtmlEscaped() + '\n';
        }
    }

    return content;
}

QString PersonDetailsWidget::generateHtml() const
{
    QString content;

    content += "<!DOCTYPE html>";
    content += "<html lang=\"en\">";
    content += "<head>";
    content += "<title>" + tr("Person details") + "</title>";
    content += "<style type=\"text/css\">";
    content += "table, th, td {\
                    border: 1px solid " + QString(this->darkMode ? "gray" : "darkgrey") + "; \
                    border-collapse: collapse; \
                    background: " + QString(this->darkMode ? "#323232" : "whitesmoke") + "; \
                    text-align: left; \
                    color: " + QString(this->darkMode ? "white" : "black") + "; \
                }";
    content += "</style>";
    content += "</head>";
    content += "<body>";

    foreach (QUuid personId, this->personsIds)
    {
        FPerson person = this->tree->findPerson(personId);
        content += "<table width=\"100%\">";
        content += "<tr><th colspan=\"2\" align=\"center\" style=\"background: " + QString(this->darkMode ? "#161616" : "lightgrey") + ";\">" + person.getName().toString() + "</th></tr>";
        content += "<tr><th>" + tr("Sex")  + "</th><td width=\"80%\">" + _FP_TR(FPerson::sexKeyToDesc(person.getSex())) + "</td></tr>";
        QString bornDateString = QLocale().toString(person.getBirth().getDate().toDate());
        QString bornPlaceString = person.getBirth().getPlace().getAddress().toString();
        QString bornSeparator = !bornDateString.isEmpty() && !bornPlaceString.isEmpty() ? ", " : QString();
        content += "<tr><th>" + tr("Born") + "</th><td>" + bornDateString + bornSeparator + bornPlaceString + "</td></tr>";
        QString deathDateString = QLocale().toString(person.getBirth().getDate().toDate());
        QString deathPlaceString = person.getBirth().getPlace().getAddress().toString();
        QString deathSeparator = !deathDateString.isEmpty() && !deathPlaceString.isEmpty() ? ", " : QString();
        content += "<tr><th>" + tr("Died") + "</th><td>" + deathDateString + deathSeparator + deathPlaceString + "</td></tr>";

        QList<QUuid> partnerRelations = this->tree->findPersonPartnerRelations(personId);
        QList<QUuid> childRelations = this->tree->findPersonChildRelations(personId);

        if (childRelations.size() > 0)
        {
            foreach (QUuid relationId, childRelations) {
                FRelation relation = this->tree->findRelation(relationId);

                content += "<tr><th colspan=\"2\" align=\"center\" style=\"background: " + QString(this->darkMode ? "#242424" : "#E8E8E8") + ";\">" + tr("Parents and siblings") + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]</th></tr>";
                content += "<tr><th>" + tr("Parents") + "</th>";
                content += "<td>";
                content += "<ul>";
                foreach (QUuid parentId, this->tree->sortPersons(relation.getPartners())) {
                    FPerson parent = this->tree->findPerson(parentId);
                    content += "<li>";
                    content += parent.getName().toString() + " [" + QLocale().toString(parent.getBirth().getDate().toDate()) + " - " + QLocale().toString(parent.getDeath().getDate().toDate()) + "]";
                    content += "</li>";
                }
                content += "</ul>";
                content += "</td>";
                content += "</tr>";

                content += "<tr><th>" + tr("Siblings") + "</th>";
                content += "<td>";
                content += "<ul>";
                foreach (QUuid childId, this->tree->sortPersons(relation.getChildren())) {
                    if (childId == personId)
                    {
                        continue;
                    }
                    FPerson child = this->tree->findPerson(childId);
                    content += "<li>";
                    content += child.getName().toString() + " [" + QLocale().toString(child.getBirth().getDate().toDate()) + " - " + QLocale().toString(child.getDeath().getDate().toDate()) + "]";
                    content += "</li>";
                }
                content += "</ul>";
                content += "</td>";
                content += "</tr>";
            }
        }

        if (partnerRelations.size() > 0)
        {
            foreach (QUuid relationId, partnerRelations) {
                FRelation relation = this->tree->findRelation(relationId);

                content += "<tr><th colspan=\"2\" align=\"center\" style=\"background: " + QString(this->darkMode ? "#242424" : "#E8E8E8") + ";\">" + tr("Partner and children") + " [" + QLocale().toString(relation.getBegin().getDate().toDate()) + " - " + QLocale().toString(relation.getEnd().getDate().toDate()) + "]</th></tr>";
                content += "<tr><th>" + tr("Partner") + "</th>";
                content += "<td>";
                content += "<ul>";
                foreach (QUuid partnerId, relation.getPartners()) {
                    if (partnerId == personId)
                    {
                        continue;
                    }
                    FPerson partner = this->tree->findPerson(partnerId);
                    content += "<li>";
                    content += partner.getName().toString() + " [" + QLocale().toString(partner.getBirth().getDate().toDate()) + " - " + QLocale().toString(partner.getDeath().getDate().toDate()) + "]";
                    content += "</li>";
                }
                content += "</ul>";
                content += "</td>";
                content += "</tr>";

                content += "<tr><th>" + tr("Children") + "</th>";
                content += "<td>";
                content += "<ul>";
                foreach (QUuid childId, relation.getChildren()) {
                    FPerson child = this->tree->findPerson(childId);
                    content += "<li>";
                    content += child.getName().toString() + " [" + QLocale().toString(child.getBirth().getDate().toDate()) + " - " + QLocale().toString(child.getDeath().getDate().toDate()) + "]";
                    content += "</li>";
                }
                content += "</ul>";
                content += "</td>";
                content += "</tr>";
            }
        }

        if (!person.getText().toHtmlEscaped().isEmpty() || !person.getPicture().isEmpty())
        {
            content += "<tr><th colspan=\"2\" align=\"center\">" + tr("Additional information") + "</th></tr>";
            if (!person.getText().toHtmlEscaped().isEmpty())
            {
                content += "<tr><td colspan=\"2\"><p style=\"white-space: pre-wrap;\">" + person.getText().toHtmlEscaped() + "</p></td></tr>";
            }
            if (!person.getPicture().isEmpty())
            {
                content += "<tr><td colspan=\"2\" align=\"left\">";
                content += "<table style=\"border: 0px\">";
                content += "<tr>";
                content += "<td style=\"border: 0px\">";
                content += "<img src=\"data:image/png;base64," + person.getPicture().getData().toBase64() + "\">";
                content += "</td>";
                content += "<td style=\"border: 0px\">";
                content += "<strong>" + person.getPicture().getTitle() + "</strong><br/>" + person.getPicture().getDescription();
                content += "</td>";
                content += "</tr>";
                content += "</table>";
                content += "</td></tr>";
            }
        }

        content += "</table>";
        content += "<br/>";
    }
    content += "</body>";
    content += "</html>";

    return content;
}

void PersonDetailsWidget::onPersonChanged(const FPerson &person)
{
    if (personsIds.contains(person.getId()))
    {
        this->populate();
    }
}
