/*******************************************************************************
Copyright (c) 2012-2016 Alex Zhondin <lexxmark.dev@gmail.com>
Copyright (c) 2015-2019 Alexandra Cherdantseva <neluhus.vagus@gmail.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

#include "PropertyQRectF.h"

#include "PropertyQRect.h"
#include "PropertyQSize.h"

QtnProperty *QtnPropertyQRectFBase::createLeftProperty(bool move)
{
	return createFieldProperty(QtnPropertyQRect::leftKey(),
		QtnPropertyQRect::leftString(), QtnPropertyQRect::leftDescriptionFmt(),
		&QRectF::left, move ? &QRectF::moveLeft : &QRectF::setLeft);
}

QtnProperty *QtnPropertyQRectFBase::createTopProperty(bool move)
{
	return createFieldProperty(QtnPropertyQRect::topKey(),
		QtnPropertyQRect::topString(), QtnPropertyQRect::topDescriptionFmt(),
		&QRectF::top, move ? &QRectF::moveTop : &QRectF::setTop);
}

QtnProperty *QtnPropertyQRectFBase::createRightProperty(bool move)
{
	return createFieldProperty(QtnPropertyQRect::rightKey(),
		QtnPropertyQRect::rightString(),
		QtnPropertyQRect::rightDescriptionFmt(), &QRectF::right,
		move ? &QRectF::moveRight : &QRectF::setRight);
}

QtnProperty *QtnPropertyQRectFBase::createBottomProperty(bool move)
{
	return createFieldProperty(QtnPropertyQRect::bottomKey(),
		QtnPropertyQRect::bottomString(),
		QtnPropertyQRect::bottomDescriptionFmt(), &QRectF::bottom,
		move ? &QRectF::moveBottom : &QRectF::setBottom);
}

QtnProperty *QtnPropertyQRectFBase::createWidthProperty()
{
	return createFieldProperty(QtnPropertyQSize::widthKey(),
		QtnPropertyQSize::widthDisplayName(),
		QtnPropertyQSize::widthDescriptionFmt(), &QRectF::width,
		&QRectF::setWidth);
}

QtnProperty *QtnPropertyQRectFBase::createHeightProperty()
{
	return createFieldProperty(QtnPropertyQSize::heightKey(),
		QtnPropertyQSize::heightDisplayName(),
		QtnPropertyQSize::heightDescriptionFmt(), &QRectF::height,
		&QRectF::setHeight);
}

QtnPropertyQRectFBase::QtnPropertyQRectFBase(QObject *parent)
	: ParentClass(parent)
{
}

bool QtnPropertyQRectFBase::fromStrImpl(
	const QString &str, QtnPropertyChangeReason reason)
{
	static QRegExp parserRect(
		"^\\s*QRectF\\s*\\(([^\\)]+)\\)\\s*$", Qt::CaseInsensitive);
	static QRegExp parserParams(
		"^\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*$",
		Qt::CaseInsensitive);

	if (!parserRect.exactMatch(str))
		return false;

	QStringList params = parserRect.capturedTexts();
	if (params.size() != 2)
		return false;

	if (!parserParams.exactMatch(params[1]))
		return false;

	params = parserParams.capturedTexts();
	if (params.size() != 5)
		return false;

	bool ok = false;
	int left = params[1].toInt(&ok);
	if (!ok)
		return false;

	int top = params[2].toInt(&ok);
	if (!ok)
		return false;

	int width = params[3].toInt(&ok);
	if (!ok)
		return false;

	int height = params[4].toInt(&ok);
	if (!ok)
		return false;

	return setValue(QRectF(left, top, width, height), reason);
}

bool QtnPropertyQRectFBase::toStrImpl(QString &str) const
{
	auto v = value();

	str = QStringLiteral("QRectF(%1, %2, %3, %4)")
			  .arg(v.left())
			  .arg(v.top())
			  .arg(v.width())
			  .arg(v.height());

	return true;
}

QtnPropertyQRectF::QtnPropertyQRectF(QObject *parent)
	: QtnSinglePropertyValue<QtnPropertyQRectFBase>(parent)
{
}

QtnPropertyQRectFCallback::QtnPropertyQRectFCallback(QObject *parent)
	: QtnSinglePropertyCallback<QtnPropertyQRectFBase>(parent)
{
}
