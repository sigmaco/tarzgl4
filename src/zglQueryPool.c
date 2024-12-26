/*
 *             :::::::::::     :::     :::::::::   ::::::::      :::
 *                 :+:       :+: :+:   :+:    :+: :+:    :+:   :+: :+:
 *                 +:+      +:+   +:+  +:+    +:+ +:+         +:+   +:+
 *                 +#+     +#++:++#++: +#++:++#:  :#:        +#++:++#++:
 *                 +#+     +#+     +#+ +#+    +#+ +#+   +#+# +#+     +#+
 *                 #+#     #+#     #+# #+#    #+# #+#    #+# #+#     #+#
 *                 ###     ###     ### ###    ###  ########  ###     ###
 *
 *                  Q W A D R O   E X E C U T I O N   E C O S Y S T E M
 *
 *                                   Public Test Build
 *                               (c) 2017 SIGMA FEDERATION
 *                             <https://sigmaco.org/qwadro/>
 */

#include "zglUtils.h"
#include "zglCommands.h"
#include "zglObjects.h"

_ZGL void _DpuBeginQuery(zglDpu* dpu, avxQueryPool qryp, afxUnit qryIdx, afxBool precise)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_QRYP, 1, &qryp);
    AFX_ASSERT_RANGE(qryp->m.cap, qryIdx, 1);
    glVmt const* gl = &dpu->gl;

    gl->BeginQuery(qryp->glTarget, qryp->glHandle[qryIdx]);
    dpu->timeElapsedQueryIdActive = qryp->glHandle[qryIdx];
}

_ZGL void _DpuEndQuery(zglDpu* dpu, avxQueryPool qryp, afxUnit queryIdx)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_QRYP, 1, &qryp);
    AFX_ASSERT_RANGE(qryp->m.cap, queryIdx, 1);
    glVmt const* gl = &dpu->gl;

    gl->EndQuery(qryp->glTarget);
    dpu->timeElapsedQueryIdActive = 0;
}

_ZGL void _DpuCopyQueryResults(zglDpu* dpu, avxQueryPool qryp, afxUnit baseQuery, afxUnit queryCnt, afxBuffer buf, afxSize offset, afxSize stride, afxQueryResultFlags flags)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_QRYP, 1, &qryp);
    AFX_ASSERT_RANGE(qryp->m.cap, baseQuery, queryCnt);
    AFX_ASSERT_OBJECTS(afxFcc_BUF, 1, &buf);
    AFX_ASSERT_RANGE(AfxGetBufferCapacity(buf, 0), offset, stride);
    glVmt const* gl = &dpu->gl;
    GLenum pname = (flags & afxQueryResultFlag_WAIT) ? GL_QUERY_RESULT : GL_QUERY_RESULT_NO_WAIT;

    //DpuBindAndSyncBuf(dpu, TRUE, TRUE, GL_QUERY_RESULT_BUFFER, );

    if (flags & afxQueryResultFlag_64)
    {
        for (afxUnit i = 0; i < qryp->m.cap; i++)
        {
            gl->GetQueryObjectui64v(qryp->glHandle[i], pname, (GLuint64[]) { offset });

            if (flags & afxQueryResultFlag_WITH_AVAIL)
            {
                GLuint avail;
                gl->GetQueryObjectuiv(qryp->glHandle[i], GL_QUERY_RESULT_AVAILABLE, &avail);
                // ??? escrever direta num offset?
            }
        }
    }
    else
    {
        for (afxUnit i = 0; i < qryp->m.cap; i++)
        {
            gl->GetQueryObjectuiv(qryp->glHandle[i], pname, (GLuint[]) { offset });

            if (flags & afxQueryResultFlag_WITH_AVAIL)
            {
                GLuint avail;
                gl->GetQueryObjectuiv(qryp->glHandle[i], GL_QUERY_RESULT_AVAILABLE, &avail);
                // ??? escrever direta num offset?
            }
        }
    }
}

_ZGL void _DpuResetQueries(zglDpu* dpu, avxQueryPool qryp, afxUnit baseQuery, afxUnit queryCnt)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_QRYP, 1, &qryp);
    AFX_ASSERT_RANGE(qryp->m.cap, baseQuery, queryCnt);
    glVmt const* gl = &dpu->gl;


}

_ZGL void _DpuWriteTimestamp(zglDpu* dpu, avxQueryPool qryp, afxUnit queryIdx, avxPipelineStage stage)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_QRYP, 1, &qryp);
    AFX_ASSERT_RANGE(qryp->m.cap, queryIdx, 1);
    glVmt const* gl = &dpu->gl;
    AFX_ASSERT(qryp->glTarget == GL_TIMESTAMP);
    gl->QueryCounter(qryp->glHandle[queryIdx], qryp->glTarget);
}

_ZGL afxError _DpuBindAndSyncQryp(zglDpu* dpu, afxBool syncOnly, avxQueryPool qryp)
{
    //AfxEntry("pip=%p", pip);
    afxError err = AFX_ERR_NONE;
    glVmt const* gl = &dpu->gl;


    if (!qryp)
    {
        
    }
    else
    {
        AFX_ASSERT_OBJECTS(afxFcc_QRYP, 1, &qryp);
        GLuint* glHandle = qryp->glHandle;

        if (/*(!glHandle) || */(qryp->updFlags & ZGL_UPD_FLAG_DEVICE))
        {
            if (/*(!glHandle) || */(qryp->updFlags & ZGL_UPD_FLAG_DEVICE_INST))
            {
                if (glHandle)
                {
                    gl->DeleteQueries(qryp->m.cap, glHandle); _ZglThrowErrorOccuried();
                    glHandle = NIL;
                }
                gl->GenQueries(qryp->m.cap, glHandle); _ZglThrowErrorOccuried();
                qryp->glHandle = glHandle;
                AfxLogEcho("Query qryp inited. %u", qryp->m.cap);
            }

            qryp->updFlags |= ~ZGL_UPD_FLAG_DEVICE;
        }
    }

    return err;
}

_ZGL afxError _ZglQrypDtor(avxQueryPool qryp)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_QRYP, 1, &qryp);
    afxDrawSystem dsys = AfxGetQueryPoolContext(qryp);

    if (qryp->glHandle)
    {
        for (afxUnit i = 0; i < qryp->m.cap; i++)
        {
            _ZglDsysEnqueueDeletion(dsys, 0, GL_QUERY_BUFFER, (afxSize)qryp->glHandle[i]);
        }
        AfxDeallocate((void**)&qryp->glHandle, AfxHere());
        qryp->glHandle = 0;
    }

    return err;
}

_ZGL afxError _ZglQrypCtor(avxQueryPool qryp, void** args, afxUnit invokeNo)
{
    afxError err = AFX_ERR_NONE;
    AFX_ASSERT_OBJECTS(afxFcc_QRYP, 1, &qryp);

    afxDrawSystem dsys = args[0];
    afxQueryType type = *(afxQueryType const*)args[1];
    afxUnit cap = *(afxUnit const*)args[2];

    AfxCoallocate(qryp->m.cap, sizeof(qryp->glHandle[0]), 0, AfxHere(), (void**)&qryp->glHandle);
    qryp->updFlags = ZGL_UPD_FLAG_DEVICE_INST;
    qryp->m.type = type;
    qryp->m.cap = cap;

    if (qryp->m.type == afxQueryType_OCCLUSION)
    {
        qryp->glTarget = GL_SAMPLES_PASSED;
    }
    else if (qryp->m.type == afxQueryType_PIPELINE)
    {
        qryp->glTarget = 0;
    }
    else if (qryp->m.type == afxQueryType_TIMESTAMP)
    {
        qryp->glTarget = GL_TIME_ELAPSED;
    }
    else AfxThrowError();

    return err;
}
