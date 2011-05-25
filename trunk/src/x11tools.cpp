////////////////////////////////////////
//  File      : x11tools.cpp          //
//  Written by: disels@gmail.com      //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "x11tools.h"

#ifndef HAVE_XKLAVIER

const char* OPTIONS_SEPARATOR = ",";

// Compiler will size array automatically.
static const char* X11DirList[] =
    {
        "/etc/X11/",
        "/usr/share/X11/",
        "/usr/local/share/X11/",
        "/usr/X11R6/lib/X11/",
        "/usr/X11R6/lib64/X11/",
        "/usr/local/X11R6/lib/X11/",
        "/usr/local/X11R6/lib64/X11/",
        "/usr/lib/X11/",
        "/usr/lib64/X11/",
        "/usr/local/lib/X11/",
        "/usr/local/lib64/X11/",
        "/usr/pkg/share/X11/",
        "/usr/pkg/xorg/lib/X11/"
    };

// Compiler will size array automatically.
static const char* rulesFileList[] =
    {
        "xkb/rules/base",
        "xkb/rules/xorg",
        "xkb/rules/xfree86"
    };

// Macro will return number of elements in any static array as long as the
// array has at least one element.
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

static const int X11_DIR_COUNT = ARRAY_SIZE(X11DirList);
static const int X11_RULES_COUNT = ARRAY_SIZE(rulesFileList);

//Unicode symbol to keysys
KeySym X11tools::unicodeToKeysys(QChar symbol)
{

 QList<unsigned short> katakanaKeysymsToUnicode;
 katakanaKeysymsToUnicode<<
    0x0000<< 0x3002<< 0x300C<< 0x300D<< 0x3001<< 0x30FB<< 0x30F2<< 0x30A1<<
    0x30A3<< 0x30A5<< 0x30A7<< 0x30A9<< 0x30E3<< 0x30E5<< 0x30E7<< 0x30C3<<
    0x30FC<< 0x30A2<< 0x30A4<< 0x30A6<< 0x30A8<< 0x30AA<< 0x30AB<< 0x30AD<<
    0x30AF<< 0x30B1<< 0x30B3<< 0x30B5<< 0x30B7<< 0x30B9<< 0x30BB<< 0x30BD<<
    0x30BF<< 0x30C1<< 0x30C4<< 0x30C6<< 0x30C8<< 0x30CA<< 0x30CB<< 0x30CC<<
    0x30CD<< 0x30CE<< 0x30CF<< 0x30D2<< 0x30D5<< 0x30D8<< 0x30DB<< 0x30DE<<
    0x30DF<< 0x30E0<< 0x30E1<< 0x30E2<< 0x30E4<< 0x30E6<< 0x30E8<< 0x30E9<<
    0x30EA<< 0x30EB<< 0x30EC<< 0x30ED<< 0x30EF<< 0x30F3<< 0x309B<< 0x309C;

QList<unsigned short> cyrillicKeysymsToUnicode;
cyrillicKeysymsToUnicode<<
    0x0000<< 0x0452<< 0x0453<< 0x0451<< 0x0454<< 0x0455<< 0x0456<< 0x0457<<
    0x0458<< 0x0459<< 0x045a<< 0x045b<< 0x045c<< 0x0000<< 0x045e<< 0x045f<<
    0x2116<< 0x0402<< 0x0403<< 0x0401<< 0x0404<< 0x0405<< 0x0406<< 0x0407<<
    0x0408<< 0x0409<< 0x040a<< 0x040b<< 0x040c<< 0x0000<< 0x040e<< 0x040f<<
    0x044e<< 0x0430<< 0x0431<< 0x0446<< 0x0434<< 0x0435<< 0x0444<< 0x0433<<
    0x0445<< 0x0438<< 0x0439<< 0x043a<< 0x043b<< 0x043c<< 0x043d<< 0x043e<<
    0x043f<< 0x044f<< 0x0440<< 0x0441<< 0x0442<< 0x0443<< 0x0436<< 0x0432<<
    0x044c<< 0x044b<< 0x0437<< 0x0448<< 0x044d<< 0x0449<< 0x0447<< 0x044a<<
    0x042e<< 0x0410<< 0x0411<< 0x0426<< 0x0414<< 0x0415<< 0x0424<< 0x0413<<
    0x0425<< 0x0418<< 0x0419<< 0x041a<< 0x041b<< 0x041c<< 0x041d<< 0x041e<<
    0x041f<< 0x042f<< 0x0420<< 0x0421<< 0x0422<< 0x0423<< 0x0416<< 0x0412<<
    0x042c<< 0x042b<< 0x0417<< 0x0428<< 0x042d<< 0x0429<< 0x0427<< 0x042a;

QList<unsigned short> greekKeysymsToUnicode;
greekKeysymsToUnicode<<
    0x0000<< 0x0386<< 0x0388<< 0x0389<< 0x038a<< 0x03aa<< 0x0000<< 0x038c<<
    0x038e<< 0x03ab<< 0x0000<< 0x038f<< 0x0000<< 0x0000<< 0x0385<< 0x2015<<
    0x0000<< 0x03ac<< 0x03ad<< 0x03ae<< 0x03af<< 0x03ca<< 0x0390<< 0x03cc<<
    0x03cd<< 0x03cb<< 0x03b0<< 0x03ce<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x0391<< 0x0392<< 0x0393<< 0x0394<< 0x0395<< 0x0396<< 0x0397<<
    0x0398<< 0x0399<< 0x039a<< 0x039b<< 0x039c<< 0x039d<< 0x039e<< 0x039f<<
    0x03a0<< 0x03a1<< 0x03a3<< 0x0000<< 0x03a4<< 0x03a5<< 0x03a6<< 0x03a7<<
    0x03a8<< 0x03a9<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x03b1<< 0x03b2<< 0x03b3<< 0x03b4<< 0x03b5<< 0x03b6<< 0x03b7<<
    0x03b8<< 0x03b9<< 0x03ba<< 0x03bb<< 0x03bc<< 0x03bd<< 0x03be<< 0x03bf<<
    0x03c0<< 0x03c1<< 0x03c3<< 0x03c2<< 0x03c4<< 0x03c5<< 0x03c6<< 0x03c7<<
    0x03c8<< 0x03c9<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000;

QList<unsigned short> technicalKeysymsToUnicode;
technicalKeysymsToUnicode<<
    0x0000<< 0x23B7<< 0x250C<< 0x2500<< 0x2320<< 0x2321<< 0x2502<< 0x23A1<<
    0x23A3<< 0x23A4<< 0x23A6<< 0x239B<< 0x239D<< 0x239E<< 0x23A0<< 0x23A8<<
    0x23AC<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x2264<< 0x2260<< 0x2265<< 0x222B<<
    0x2234<< 0x221D<< 0x221E<< 0x0000<< 0x0000<< 0x2207<< 0x0000<< 0x0000<<
    0x223C<< 0x2243<< 0x0000<< 0x0000<< 0x0000<< 0x21D4<< 0x21D2<< 0x2261<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x221A<< 0x0000<<
    0x0000<< 0x0000<< 0x2282<< 0x2283<< 0x2229<< 0x222A<< 0x2227<< 0x2228<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x2202<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0192<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x2190<< 0x2191<< 0x2192<< 0x2193<< 0x0000;

QList<unsigned short> specialKeysymsToUnicode;
specialKeysymsToUnicode<<
    0x25C6<< 0x2592<< 0x2409<< 0x240C<< 0x240D<< 0x240A<< 0x0000<< 0x0000<<
    0x2424<< 0x240B<< 0x2518<< 0x2510<< 0x250C<< 0x2514<< 0x253C<< 0x23BA<<
    0x23BB<< 0x2500<< 0x23BC<< 0x23BD<< 0x251C<< 0x2524<< 0x2534<< 0x252C<<
    0x2502<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000;

QList<unsigned short> publishingKeysymsToUnicode;
    publishingKeysymsToUnicode<<
    0x0000<< 0x2003<< 0x2002<< 0x2004<< 0x2005<< 0x2007<< 0x2008<< 0x2009<<
    0x200a<< 0x2014<< 0x2013<< 0x0000<< 0x0000<< 0x0000<< 0x2026<< 0x2025<<
    0x2153<< 0x2154<< 0x2155<< 0x2156<< 0x2157<< 0x2158<< 0x2159<< 0x215a<<
    0x2105<< 0x0000<< 0x0000<< 0x2012<< 0x2329<< 0x0000<< 0x232a<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x215b<< 0x215c<< 0x215d<< 0x215e<< 0x0000<<
    0x0000<< 0x2122<< 0x2613<< 0x0000<< 0x25c1<< 0x25b7<< 0x25cb<< 0x25af<<
    0x2018<< 0x2019<< 0x201c<< 0x201d<< 0x211e<< 0x0000<< 0x2032<< 0x2033<<
    0x0000<< 0x271d<< 0x0000<< 0x25ac<< 0x25c0<< 0x25b6<< 0x25cf<< 0x25ae<<
    0x25e6<< 0x25ab<< 0x25ad<< 0x25b3<< 0x25bd<< 0x2606<< 0x2022<< 0x25aa<<
    0x25b2<< 0x25bc<< 0x261c<< 0x261e<< 0x2663<< 0x2666<< 0x2665<< 0x0000<<
    0x2720<< 0x2020<< 0x2021<< 0x2713<< 0x2717<< 0x266f<< 0x266d<< 0x2642<<
    0x2640<< 0x260e<< 0x2315<< 0x2117<< 0x2038<< 0x201a<< 0x201e<< 0x0000;

QList<unsigned short> aplKeysymsToUnicode;
    aplKeysymsToUnicode<<
    0x0000<< 0x0000<< 0x0000<< 0x003c<< 0x0000<< 0x0000<< 0x003e<< 0x0000<<
    0x2228<< 0x2227<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x00af<< 0x0000<< 0x22a5<< 0x2229<< 0x230a<< 0x0000<< 0x005f<< 0x0000<<
    0x0000<< 0x0000<< 0x2218<< 0x0000<< 0x2395<< 0x0000<< 0x22a4<< 0x25cb<<
    0x0000<< 0x0000<< 0x0000<< 0x2308<< 0x0000<< 0x0000<< 0x222a<< 0x0000<<
    0x2283<< 0x0000<< 0x2282<< 0x0000<< 0x22a2<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x0000<<
    0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x22a3<< 0x0000<< 0x0000<< 0x0000;

QList<unsigned short> koreanKeysymsToUnicode;
     koreanKeysymsToUnicode <<
    0x0000<< 0x3131<< 0x3132<< 0x3133<< 0x3134<< 0x3135<< 0x3136<< 0x3137<<
    0x3138<< 0x3139<< 0x313a<< 0x313b<< 0x313c<< 0x313d<< 0x313e<< 0x313f<<
    0x3140<< 0x3141<< 0x3142<< 0x3143<< 0x3144<< 0x3145<< 0x3146<< 0x3147<<
    0x3148<< 0x3149<< 0x314a<< 0x314b<< 0x314c<< 0x314d<< 0x314e<< 0x314f<<
    0x3150<< 0x3151<< 0x3152<< 0x3153<< 0x3154<< 0x3155<< 0x3156<< 0x3157<<
    0x3158<< 0x3159<< 0x315a<< 0x315b<< 0x315c<< 0x315d<< 0x315e<< 0x315f<<
    0x3160<< 0x3161<< 0x3162<< 0x3163<< 0x11a8<< 0x11a9<< 0x11aa<< 0x11ab<<
    0x11ac<< 0x11ad<< 0x11ae<< 0x11af<< 0x11b0<< 0x11b1<< 0x11b2<< 0x11b3<<
    0x11b4<< 0x11b5<< 0x11b6<< 0x11b7<< 0x11b8<< 0x11b9<< 0x11ba<< 0x11bb<<
    0x11bc<< 0x11bd<< 0x11be<< 0x11bf<< 0x11c0<< 0x11c1<< 0x11c2<< 0x316d<<
    0x3171<< 0x3178<< 0x317f<< 0x3181<< 0x3184<< 0x3186<< 0x318d<< 0x318e<<
    0x11eb<< 0x11f0<< 0x11f9<< 0x0000<< 0x0000<< 0x0000<< 0x0000<< 0x20a9;

    int tempCodeNumber;
    unsigned char codeNumber ;
    unsigned char localeNumber;
    unsigned short  unc = symbol.unicode();

 localeNumber = 0x04;
 tempCodeNumber= katakanaKeysymsToUnicode.indexOf(unc);
if (tempCodeNumber<0)
    {
        localeNumber = 0x06;
        tempCodeNumber= cyrillicKeysymsToUnicode.indexOf(unc);
    }
if (tempCodeNumber<0)
    {
       localeNumber = 0x07;
       tempCodeNumber= greekKeysymsToUnicode.indexOf(unc);
    }

if (tempCodeNumber<0)
    {
       localeNumber = 0x08;
       codeNumber= technicalKeysymsToUnicode.indexOf(unc);
    }

if (tempCodeNumber<0)
    {
       localeNumber = 0x09;
       tempCodeNumber= specialKeysymsToUnicode.indexOf(unc);
    }

if (tempCodeNumber<0)
    {
       localeNumber = 0x0a;
       tempCodeNumber= publishingKeysymsToUnicode.indexOf(unc);
    }

if (tempCodeNumber<0)
    {
       localeNumber = 0x0b;
       tempCodeNumber= aplKeysymsToUnicode.indexOf(unc);
    }
if (tempCodeNumber<0)
    {
       localeNumber = 0x0e;
       tempCodeNumber= koreanKeysymsToUnicode.indexOf(unc);
    }

if (tempCodeNumber<0)
        return (-1);
else
    {
        codeNumber = (unsigned char) tempCodeNumber;
        return (localeNumber<<8 |(codeNumber+0xa0));
    }
}
//
X11tools::X11tools()
{
}

const QString X11tools::findX11Dir()
{
        for(int ii=0; ii<X11_DIR_COUNT; ii++) {
                const char* xDir = X11DirList[ii];
                if( xDir != NULL && QDir(QString(xDir) + "xkb/rules").exists() ) {
                        return QString(xDir);
            }
        }
        return NULL;
}

const QString X11tools::findXkbRulesFile(const QString &x11Dir,Display *dpy)
{
        QString rulesFile;
        XkbRF_VarDefsRec vd;
        char *tmp = NULL;

        if (XkbRF_GetNamesProp(dpy, &tmp, &vd) && tmp != NULL ) {
// 			qDebug() << "namesprop " << tmp ;
                rulesFile = x11Dir + QString("xkb/rules/%1").arg(tmp);
// 			qDebug() << "rulesF " << rulesFile ;
        }
        else {
    // old way
        for(int ii=0; ii<X11_RULES_COUNT; ii++) {
                const char* ruleFile = rulesFileList[ii];
                QString xruleFilePath = x11Dir + ruleFile;
                        qDebug() << "trying xrules path " << xruleFilePath;
                    if( QFile(xruleFilePath).exists() ) {
                                rulesFile = xruleFilePath;
                        break;
                    }
        }
    }

        return rulesFile;
}

RulesInfo* X11tools::loadRules(const QString& file, bool layoutsOnly)
{
        XkbRF_RulesPtr xkbRules = XkbRF_Load(QFile::encodeName(file).data(), (char*)"", true, true);
        qDebug() << " X11tools::loadRule " ;
        if (xkbRules == NULL) {
// throw Exception
                return NULL;
        }

    // try to translate layout names by countries in desktop_kdebase
    // this is poor man's translation as it's good only for layout names and only those which match country names

        RulesInfo* rulesInfo = new RulesInfo();

        for (int i = 0; i < xkbRules->layouts.num_desc; ++i) {
                QString layoutName(xkbRules->layouts.desc[i].name);
                rulesInfo->layouts.insert( layoutName, xkbRules->layouts.desc[i].desc);
        }

        if( layoutsOnly == true ) {
                XkbRF_Free(xkbRules, true);
                return rulesInfo;
        }

    for (int i = 0; i < xkbRules->models.num_desc; ++i)
        rulesInfo->models.insert(xkbRules->models.desc[i].name, QString( xkbRules->models.desc[i].desc ) );

    for (int i = 0; i < xkbRules->options.num_desc; ++i) {
        QString optionName = xkbRules->options.desc[i].name;
        //qDebug() << " option: " << optionName;
        int colonPos = optionName.indexOf(':');
        QString groupName = optionName.mid(0, colonPos);


        if( colonPos != -1 ) {
            //qDebug() << " option: " << optionName;

            if( ! rulesInfo->optionGroups.contains( groupName ) ) {
                rulesInfo->optionGroups.insert(groupName, createMissingGroup(groupName));
              //  qDebug() << " added missing option group: " << groupName;
            }

            XkbOption option;
            option.name = optionName;
            option.description = xkbRules->options.desc[i].desc;
            //option.group = &rulesInfo->optionGroups[ groupName ];
            rulesInfo->optionGroups[ groupName ].options <<option;
            rulesInfo->options.insert(optionName, option);
        }
        else {
            if( groupName == "Compose" )
                groupName = "compose";
            if( groupName == "compat" )
                groupName = "numpad";

            XkbOptionGroup optionGroup;
            optionGroup.name = groupName;
            optionGroup.description = xkbRules->options.desc[i].desc;
            optionGroup.exclusive = isGroupExclusive( groupName );

            //qDebug() << " option group: " << groupName;
            rulesInfo->optionGroups.insert(groupName, optionGroup);
        }
  }

  XkbRF_Free(xkbRules, true);

  return rulesInfo;
}


XkbOptionGroup X11tools::createMissingGroup(const QString& groupName)
{
// workaround for empty 'compose' options group description
   XkbOptionGroup optionGroup;
   optionGroup.name = groupName;
//   optionGroup.description = "";
   optionGroup.exclusive = isGroupExclusive( groupName );

   return optionGroup;
}

bool X11tools::isGroupExclusive(const QString& groupName)
{
    if( groupName == "ctrl" || groupName == "keypad" || groupName == "nbsp"
            || groupName == "kpdl" || groupName == "caps" || groupName == "altwin" )
        return true;

    return false;
}


/* pretty simple algorithm - reads the layout file and
    tries to find "xkb_symbols"
    also checks whether previous line contains "hidden" to skip it
*/

QList<XkbVariant> X11tools::getVariants(const QString& layout, const QString& x11Dir)
{
  QList<XkbVariant>* result = new QList<XkbVariant>();

  QString file = x11Dir + "xkb/symbols/";
  // workaround for XFree 4.3 new directory for one-group layouts
  if( QDir(file+"pc").exists() )
    file += "pc/";

  file += layout;

//  qDebug() << "reading variants from " << file;

  QFile f(file);
  if (f.open(QIODevice::ReadOnly))
    {
      QTextStream ts(&f);

      QString line;
      QString prev_line;

          while ( ts.status() == QTextStream::Ok ) {
        prev_line = line;

                QString str = ts.readLine();
                if( str.isNull() )
                  break;

                line = str.simplified();

            if (line[0] == '#' || line.left(2) == "//" || line.isEmpty())
                continue;

            int pos = line.indexOf("xkb_symbols");
            if (pos < 0)
                continue;

            if( prev_line.indexOf("hidden") >=0 )
                continue;

            pos = line.indexOf('"', pos) + 1;
            int pos2 = line.indexOf('"', pos);
            if( pos < 0 || pos2 < 0 )
                continue;

            XkbVariant variant;
            variant.name = line.mid(pos, pos2-pos);
            variant.description = line.mid(pos, pos2-pos);
            result->append(variant);
//  qDebug() << "adding variant " << line.mid(pos, pos2-pos);
      }

      f.close();
    }

    return *result;
}

XKBConf X11tools::getGroupNames(Display* dpy)
{
    Atom real_prop_type;
    int fmt;
    unsigned long nitems, extra_bytes;
    char *prop_data = NULL;
    Status ret;
    XKBConf xkbConfig;

    Atom rules_atom = XInternAtom(dpy, _XKB_RF_NAMES_PROP_ATOM, False);

    /* no such atom! */
    if (rules_atom == None) {       /* property cannot exist */
       qCritical() << "X11tools:Failed to fetch layouts from server:" << "could not find the atom" << _XKB_RF_NAMES_PROP_ATOM;
        return xkbConfig;
    }

    ret = XGetWindowProperty(dpy,
                   QX11Info::appRootWindow(),
                   rules_atom, 0L, _XKB_RF_NAMES_PROP_MAXLEN,
                   False, XA_STRING, &real_prop_type, &fmt,
                   &nitems, &extra_bytes,
                   (unsigned char **) (void *) &prop_data);

    /* property not found! */
    if (ret != Success) {
       qCritical() << "X11tools:Failed to fetch layouts from server:" << "Could not get the property";
        return xkbConfig;
    }

    /* has to be array of strings */
    if ((extra_bytes > 0) || (real_prop_type != XA_STRING)
                || (fmt != 8)) {
        if (prop_data)
            XFree(prop_data);
       qCritical() << "X11tools:Failed to fetch layouts from server:" << "Wrong property format";
        return xkbConfig;
    }

    qDebug() << "prop_data:" << nitems << prop_data;
    QStringList names;
    for(char* p=prop_data; p-prop_data < (long)nitems && p != NULL; p += strlen(p)+1) {
        names.append( p );
        qDebug() << " " << p;
    }

    if( names.count() >= 4 ) { //{ rules, model, layouts, variants, options }
        xkbConfig.model = names[1];
//        qDebug() << "model:" << xkbConfig.model;

        QStringList layouts = names[2].split(OPTIONS_SEPARATOR);
        QStringList variants = names[3].split(OPTIONS_SEPARATOR);

        for(int ii=0; ii<layouts.count(); ii++) {
            LayoutUnit lu;
            lu.layout = layouts[ii];
            lu.variant = ii < variants.count() ? variants[ii] : "";
            xkbConfig.layouts << lu;
            qDebug() << "X11tools:layout nm:" << lu.layout << "variant:" << lu.variant;
        }

        if( names.count() >= 5 ) {
            QString options = names[4];
            xkbConfig.options = options.split(OPTIONS_SEPARATOR);
            qDebug() << "X11tools:options:" << options;
        }
    }

    XFree(prop_data);
    return xkbConfig;
}

XKBConf* X11tools::loadXKBconf()
{
    XKBConf* Conf = new XKBConf();
    LayoutUnit lu;
    QSettings *antico = new QSettings(QDir::homePath() + "/.config/qxkb.cfg", QSettings::IniFormat);
    antico->beginGroup("KeyLayout");
    Conf->model = antico->value("model").toString();
    if (Conf->model.isEmpty())
        Conf->model="pc104";
    if (!antico->value("layout").toString().isEmpty()&& !antico->value("layout").toString().isNull())
    {
        QStringList l = antico->value("layout").toString().split(",");;
        QStringList v = antico->value("variant").toString().split(",");;
         for(int i =0 ;i<l.size();i++)
         {
               lu.layout=l[i];

               if (v.size()<i)
                   lu.variant="";
               else
                   lu.variant=v[i];
               Conf->layouts.append(lu);
         }
     }
    else
    {
        lu.layout="us";
        lu.variant=" ";
        Conf->layouts.append(lu);
    }
    if (!antico->value("option").toString().isEmpty())
        Conf->options = antico->value("option").toString().split(",");
    Conf->status = antico->value("status").toInt();
    Conf->showFlag= antico->value("showflag").toBool();
    Conf->showSingle= antico->value("showsingle").toBool();
    Conf->switching= antico->value("switching").toInt();
    Conf->useConvert= antico->value("useConvert").toBool();
    Conf->shotcutConvert= antico->value("shotcutConvert").toString();
    Conf->lockKeys=false;
    return Conf;
}

void X11tools::saveXKBconf(XKBConf* conf)
{

    LayoutUnit lu;
    QSettings *antico = new QSettings(QDir::homePath() + "/.config/qxkb.cfg", QSettings::IniFormat);
    antico->beginGroup("KeyLayout");
    antico->setValue("model",conf->model);
    QString layout;
    QString variant;

    for(int i=0;i<conf->layouts.size();i++)
    {
        layout+=conf->layouts[i].layout;
        if (i<conf->layouts.size()-1)
            layout+=",";
        variant+=conf->layouts[i].variant;
        if (i<conf->layouts.size()-1)
            variant+=",";
     }
     antico->setValue("layout",layout);
     antico->setValue("variant",variant);
     antico->setValue("status",conf->status);
     antico->setValue("showflag",conf->showFlag);
     antico->setValue("showsingle",conf->showSingle);
     antico->setValue("switching",conf->switching);
     antico->setValue("useConvert",conf->useConvert);
     antico->setValue("shotcutConvert",conf->shotcutConvert);
     QString option= conf->options.join(",");
     antico->setValue("option",option);

}

QString X11tools::get_selected_text(XSelectionEvent *event)
{
        if (event->property == None)
        {
                qDebug()<<"X11tools:X11tools:Convert to selection target return None answer";
                return NULL;
        }
        unsigned long len, bytes_left, dummy;
        unsigned char *data = NULL;
        int format;
        Atom type;

        XGetWindowProperty(event->display, event->requestor, event->property, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes_left, &data);
        if (bytes_left == 0)
        {
                qDebug()<<"X11tools:Selected text length is 0";
                return NULL;
        }

        if (XGetWindowProperty(event->display, event->requestor, event->property, 0, bytes_left, 0, AnyPropertyType, &type,&format, &len, &dummy, &data) != Success)
        {
                qDebug()<<"X11tools:Failed to get selected text data";
                return NULL;
        }

        return  QString((char*) data);
}


Window X11tools::getActiveWindowId()
{
    Display* disp = QX11Info::display();
    Window focus_return;
    int revert_to_return;
    XGetInputFocus(disp, &focus_return, &revert_to_return); // get input focus
    return focus_return;
}

QString X11tools::getActiveWindowAppName(Window windowsId)
{
   char **window_name_return;
   window_name_return=(char**) malloc(50);
   XFetchName(QX11Info::display(), windowsId, window_name_return);
   QString win_name = QString(*window_name_return);
   QStringList tmp = win_name.split("-");
   if (tmp.count()>1) win_name= tmp[tmp.count()-1];
   tmp = win_name.split(":");
   if (tmp.count()>1) win_name= tmp[tmp.count()-1];
   if (win_name.isEmpty() || win_name.isNull())  win_name="NONE";
   win_name.trimmed();
   return win_name;
}


#endif  /* HAVE_XKLAVIER*/

QString LayoutUnit::getDefaultDisplayName(const QString& layout, const QString& /*variant*/)
{
    return layout.left(MAX_LABEL_LEN);

}

/**
 * @brief Gets the single layout part of a layout(variant) string
 * @param[in] layvar String in form layout(variant) to parse
 * @return The layout found in the string
 */
const QString LayoutUnit::parseLayout(const QString &layvar)
{
        static const char* LAYOUT_PATTERN = "[a-zA-Z0-9_/-]*";
        QString varLine = layvar.trimmed();
        QRegExp rx(LAYOUT_PATTERN);
        int pos = rx.indexIn(varLine, 0);
        int len = rx.matchedLength();
  // check for errors
        if( pos < 0 || len < 2 )
                return "";
        return varLine.mid(pos, len);
}

/**
 * @brief Gets the single variant part of a layout(variant) string
 * @param[in] layvar String in form layout(variant) to parse
 * @return The variant found in the string, no check is performed
 */
const QString LayoutUnit::parseVariant(const QString &layvar)
{
        static const char* VARIANT_PATTERN = "\\([a-zA-Z0-9_-]*\\)";
        QString varLine = layvar.trimmed();
        QRegExp rx(VARIANT_PATTERN);
        int pos = rx.indexIn(varLine, 0);
        int len = rx.matchedLength();
  // check for errors
        if( pos < 2 || len < 2 )
                return "";
        return varLine.mid(pos+1, len-2);
}





